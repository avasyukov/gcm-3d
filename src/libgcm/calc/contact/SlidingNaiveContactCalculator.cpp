#include "libgcm/calc/contact/SlidingNaiveContactCalculator.hpp"
#include "libgcm/util/StressTensor.hpp"

//#include "libgcm/Math.hpp"


using namespace gcm;
using std::vector;

SlidingNaiveContactCalculator::SlidingNaiveContactCalculator()
{
    INIT_LOGGER("gcm.SlidingNaiveContactCalculator");
    type = "SlidingNaiveContactCalculator";

    fbc = new FreeNaiveBorderCalculator();
};

SlidingNaiveContactCalculator::~SlidingNaiveContactCalculator()
{
    delete fbc;
};

void SlidingNaiveContactCalculator::doCalc(CalcNode& cur_node, CalcNode& new_node, CalcNode& virt_node,
                            RheologyMatrixPtr matrix, vector<CalcNode>& previousNodes, bool inner[],
                            RheologyMatrixPtr virt_matrix, vector<CalcNode>& virtPreviousNodes, bool virt_inner[],
                            float outer_normal[], float scale)
{
    assert_eq(previousNodes.size(), 9);
    assert_eq(virtPreviousNodes.size(), 9);

    if (isFreeBorder(cur_node, virt_node, outer_normal))
    {
        fbc->doCalc(cur_node, new_node, matrix, previousNodes, inner, outer_normal, scale);
        return;
    }

    real nv0 =  outer_normal[0] * previousNodes[0].vx + 
                outer_normal[1] * previousNodes[0].vy + 
                outer_normal[2] * previousNodes[0].vz;
    real nv1 =  outer_normal[0] * virt_node.vx + 
                outer_normal[1] * virt_node.vy + 
                outer_normal[2] * virt_node.vz;
    
    new_node.vx = previousNodes[0].vx - 0.5 * outer_normal[0] * (nv0 - nv1);
    new_node.vy = previousNodes[0].vy - 0.5 * outer_normal[1] * (nv0 - nv1);
    new_node.vz = previousNodes[0].vz - 0.5 * outer_normal[2] * (nv0 - nv1);

    virt_node.vx = virt_node.vx - 0.5 * outer_normal[0] * (nv1 - nv0);
    virt_node.vy = virt_node.vy - 0.5 * outer_normal[1] * (nv1 - nv0);
    virt_node.vz = virt_node.vz - 0.5 * outer_normal[2] * (nv1 - nv0);

    float local_n[3][3];
	local_n[0][0] = outer_normal[0];
	local_n[0][1] = outer_normal[1];
	local_n[0][2] = outer_normal[2];
    createLocalBasis(local_n[0], local_n[1], local_n[2]);
    gcm::linal::Matrix33 tobasis;
    tobasis.a11 = local_n[0][0]; tobasis.a21 = local_n[0][1]; tobasis.a31 = local_n[0][2];
    tobasis.a12 = local_n[1][0]; tobasis.a22 = local_n[1][1]; tobasis.a32 = local_n[1][2];
    tobasis.a13 = local_n[2][0]; tobasis.a23 = local_n[2][1]; tobasis.a33 = local_n[2][2];

    StressTensor s0(previousNodes[0].sxx, previousNodes[0].sxy, previousNodes[0].sxz, 
                        previousNodes[0].syy, previousNodes[0].syz, previousNodes[0].szz),
                 s1(virt_node.sxx, virt_node.sxy, virt_node.sxz, 
                        virt_node.syy, virt_node.syz, virt_node.szz);
    StressTensor s0r(s0), s1r(s1);
    s0r.transform(tobasis);
    s1r.transform(tobasis);
    
    StressTensor s0new((s0r.xx + s1r.xx)/2.0, (s0r.xy + s1r.xy)/2.0, (s0r.xz + s1r.xz)/2.0,
                        - s0r.yy, - s0r.yz, - s0r.yy);
    StressTensor s1new((s0r.xx + s1r.xx)/2.0, (s0r.xy + s1r.xy)/2.0, (s0r.xz + s1r.xz)/2.0,
                        - s1r.yy, - s1r.yz, - s1r.zz);
	
    tobasis.transpose();
    s0new.transform(tobasis);
    s1new.transform(tobasis);

    new_node.sxx = s0new.xx;
    new_node.sxy = s0new.xy;
    new_node.sxz = s0new.xz;
    new_node.syy = s0new.yy;
    new_node.syz = s0new.yz;
    new_node.szz = s0new.zz;

    virt_node.sxx = s1new.xx;
    virt_node.sxy = s1new.xy;
    virt_node.sxz = s1new.xz;
    virt_node.syy = s1new.yy;
    virt_node.syz = s1new.yz;
    virt_node.szz = s1new.zz;

	/*if (isFreeBorder(new_node, virt_node, outer_normal))
    {
        fbc->doCalc(cur_node, new_node, matrix, previousNodes, inner, outer_normal, scale);
        return;
    }*/

};

bool SlidingNaiveContactCalculator::isFreeBorder(CalcNode& cur_node, 
	CalcNode& virt_node, float outer_normal[])
{	
	float local_n[3][3];
    local_n[0][0] = outer_normal[0];
    local_n[0][1] = outer_normal[1];
    local_n[0][2] = outer_normal[2];

    createLocalBasis(local_n[0], local_n[1], local_n[2]);

    //---------------------------------------Check if nodes fall apart
    LOG_TRACE("Cur node: " << cur_node);
    LOG_TRACE("Virt node: " << virt_node);

    float vel_rel[3] = {
        cur_node.vx - virt_node.vx,
        cur_node.vy - virt_node.vy,
        cur_node.vz - virt_node.vz
    };
    float vel_avg[3] = {
        cur_node.vx + virt_node.vx,
        cur_node.vy + virt_node.vy,
        cur_node.vz + virt_node.vz
    };
    float vel_avg_abs = sqrt(scalarProduct(vel_avg, vel_avg));
    float vel_rel_abs = sqrt(scalarProduct(vel_rel, vel_rel));
    
    float force_cur[3] = {
        cur_node.sxx*outer_normal[0] + cur_node.sxy*outer_normal[1] + cur_node.sxz*outer_normal[2],
        cur_node.sxy*outer_normal[0] + cur_node.syy*outer_normal[1] + cur_node.syz*outer_normal[2],
        cur_node.sxz*outer_normal[0] + cur_node.syz*outer_normal[1] + cur_node.szz*outer_normal[2]
    };
    float force_virt[3] = {
        virt_node.sxx*outer_normal[0] + virt_node.sxy*outer_normal[1] + virt_node.sxz*outer_normal[2],
        virt_node.sxy*outer_normal[0] + virt_node.syy*outer_normal[1] + virt_node.syz*outer_normal[2],
        virt_node.sxz*outer_normal[0] + virt_node.syz*outer_normal[1] + virt_node.szz*outer_normal[2]
    };

    float vel_rel_p = scalarProduct(vel_rel, outer_normal);
    float vel_avg_p = scalarProduct(vel_avg, outer_normal);
    float force_cur_p = scalarProduct(force_cur,outer_normal);
    float force_virt_p = scalarProduct(force_virt,outer_normal);
    float force_rel_p = force_cur_p + force_virt_p;


    LOG_TRACE("Vrel: " << vel_rel[0] << " " << vel_rel[1] << " " << vel_rel[2]);
    LOG_TRACE("Fcur: " << force_cur[0] << " " << force_cur[1] << " " << force_cur[2]);
    LOG_TRACE("Vavg: " << vel_avg_abs << " Vdelta: " << vel_rel_abs);
    LOG_TRACE("VrelP: " << vel_rel_p << " Fabs: " << force_rel_p);
    
    bool free_border = false;
    float eps = 0.2*fabs(vel_avg_p);
    // If relative speed is positive
    if(vel_rel_p < -eps) {
    	free_border = true;
    } else if (vel_rel_p < eps) { 
        // If relative speed is close to zero, check force
        if (force_rel_p > 0)
		{
            free_border = true;
		}
    } 
    
    LOG_TRACE("Free border: " << free_border);
	return free_border;
};
