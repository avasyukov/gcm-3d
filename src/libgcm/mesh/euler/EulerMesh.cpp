#include "libgcm/mesh/euler/EulerMesh.hpp"

#include "libgcm/util/Assertion.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/Math.hpp"
#include "libgcm/snapshot/VTKEulerMeshSnapshotWriter.hpp"

#include <algorithm>
#include <cmath>

using namespace gcm;
using std::min;
using std::floor;

const uchar EulerMesh::VIRT_FLAG = CalcNode::FLAG_1;
const uchar EulerMesh::VIRT_CELL_AXIS_X_FLAG = CalcNode::FLAG_2;
const uchar EulerMesh::VIRT_CELL_AXIS_Y_FLAG = CalcNode::FLAG_3;
const uchar EulerMesh::VIRT_CELL_AXIS_Z_FLAG = CalcNode::FLAG_4;

EulerMesh::EulerMesh()
{
    INIT_LOGGER("gcm.mesh.euler.EulerMesh");
    numericalMethodType = "InterpolationFixedAxis";
}

EulerMesh::~EulerMesh()
{
    if (cellStatus)
    {
        for (uint i = 0; i < dimensions.x; i++)
        {
            for (uint j = 0; j < dimensions.y; j++)
                delete[] cellStatus[i][j];
            delete[] cellStatus[i];
        }
        delete[] cellStatus;
    }
}

EulerMesh::EulerMesh(vector3u dimensions, vector3r cellSize, vector3r center): EulerMesh()
{
    setDimensions(dimensions);
    setCellSize(cellSize);
    setCenter(center);
    generateMesh();

    for (int i = 0; i < dimensions.x; i++)
        for (int j = 0; j < dimensions.y; j++)
            for (int k = 0; k < dimensions.z; k++)
                cellStatus[i][j][k] = true;

    auto& n1 = getNodeByEulerMeshIndex(vector3u(0, 0, 0));
    auto& n2 = getNodeByEulerMeshIndex(dimensions-vector3u(1, 1, 1));
    outline.minX = n1.coords.x;
    outline.minY = n1.coords.y;
    outline.minZ = n1.coords.z;
    outline.maxX = n2.coords.x;
	outline.maxY = n2.coords.y;
	outline.maxZ = n2.coords.z;
}

void EulerMesh::calcMinH()
{
    minH = min({cellSize.x, cellSize.y, cellSize.z});
}

float EulerMesh::getMinH()
{
    return minH;
}

float EulerMesh::getRecommendedTimeStep()
{
    if (tau < 0) tau = getMinH()/getMaxEigenvalue();
    return tau;
}

void EulerMesh::doNextPartStep(float tau, int stage)
{
    defaultNextPartStep(tau, stage);
}

const SnapshotWriter& EulerMesh::getSnaphotter() const
{
    return VTKEulerMeshSnapshotWriter::getInstance();
}

const SnapshotWriter& EulerMesh::getDumper() const
{
    return VTKEulerMeshSnapshotWriter::getInstance();
}

void EulerMesh::findBorderNodeNormal(const CalcNode& node, float* x, float* y, float* z, bool debug)
{
    assert_true(borderNormals.find(node.number) != borderNormals.end(),
        {
            LOG_DEBUG(node);
        }
    );

    auto norm = borderNormals[node.number];
    *x = norm.x;
    *y = norm.y;
    *z = norm.z;
}

bool EulerMesh::interpolateBorderNode(real x, real y, real z, real dx, real dy, real dz, CalcNode& node)
{
    assert_true(
        ((dx == 0.0) && (dy == 0.0)) ||
        ((dy == 0.0) && (dz == 0.0)) ||
        ((dx == 0.0) && (dz == 0.0))
    );

    vector3r coords(x, y, z);

    auto index0 = getCellEulerIndexByCoordsUnsafe(coords);

    coords += vector3r(dx, dy, dz);
    auto index = getCellEulerIndexByCoordsUnsafe(coords);


    auto dindex = index - index0;
    int l = dindex.normalize();


    bool found = false;
    vector3i cmin(0, 0, 0);
    vector3i cmax(dimensions.x-1, dimensions.y-1, dimensions.z-1);
    for (int i = 0; i <= l; i++)
    {
    	if ((index0 >>= cmin) && (index0 <<= cmax))
			if (cellStatus[index0.x][index0.y][index0.z])
			{
				found = true;
				break;
			}
    	index0 += dindex;
    }

    if (!found)
    	return false;

    index = index0;

    real x1, x2, y1, y2, _x, _y;
    real* q11;
    real* q21;
    real* q22;
    real* q12;

    uint d;

    if (dx != 0.0)
    {
        d = dx > 0.0 ? 0 : 1;
        auto& _node = getNodeByEulerMeshIndex(vector3u(index.x+d, index.y, index.z));
        assert_true(_node.isBorder());

        x1 = _node.coords.y;
        y1 = _node.coords.z;
        x2 = x1 + cellSize.y;
        y2 = y1 + cellSize.z;

        q11 = _node.values;
        q21 = getNodeByEulerMeshIndex(vector3u(index.x+d, index.y+1, index.z)).values;
        q22 = getNodeByEulerMeshIndex(vector3u(index.x+d, index.y+1, index.z+1)).values;
        q12 = getNodeByEulerMeshIndex(vector3u(index.x+d, index.y, index.z+1)).values;

        node.coords.x = _node.coords.x;
        _x = node.coords.y = coords.y;
        _y = node.coords.z = coords.z;

        node.setMaterialId(_node.getMaterialId());
        node.setRheologyMatrix(_node.getRheologyMatrix());
        node.number = _node.number;

        node.setCustomFlag(VIRT_CELL_AXIS_X_FLAG, d);
        node.setCustomFlag(VIRT_CELL_AXIS_Y_FLAG, 0);
        node.setCustomFlag(VIRT_CELL_AXIS_Z_FLAG, 0);
    } else if (dy != 0.0)
    {
        d = dy > 0.0 ? 0 : 1;
        auto& _node = getNodeByEulerMeshIndex(vector3u(index.x, index.y+d, index.z));
        assert_true(_node.isBorder());

        x1 = _node.coords.x;
        y1 = _node.coords.z;
        x2 = x1 + cellSize.x;
        y2 = y1 + cellSize.z;

        q11 = _node.values;
        q21 = getNodeByEulerMeshIndex(vector3u(index.x+1, index.y+d, index.z)).values;
        q22 = getNodeByEulerMeshIndex(vector3u(index.x+1, index.y+d, index.z+1)).values;
        q12 = getNodeByEulerMeshIndex(vector3u(index.x, index.y+d, index.z+1)).values;

        _x = node.coords.x = coords.x;
        node.coords.y = _node.coords.y;
        _y = node.coords.z = coords.z;

        node.setMaterialId(_node.getMaterialId());
        node.setRheologyMatrix(_node.getRheologyMatrix());
        node.number = _node.number;

        node.setCustomFlag(VIRT_CELL_AXIS_X_FLAG, 0);
        node.setCustomFlag(VIRT_CELL_AXIS_Y_FLAG, d);
        node.setCustomFlag(VIRT_CELL_AXIS_Z_FLAG, 0);
    } else
    {
        d = dz > 0.0 ? 0 : 1;
        auto& _node = getNodeByEulerMeshIndex(vector3u(index.x, index.y, index.z+d));
        assert_true(_node.isBorder());

        x1 = _node.coords.x;
        y1 = _node.coords.y;
        x2 = x1 + cellSize.x;
        y2 = y1 + cellSize.y;

        q11 = _node.values;
        q21 = getNodeByEulerMeshIndex(vector3u(index.x+1, index.y, index.z+d)).values;
        q22 = getNodeByEulerMeshIndex(vector3u(index.x+1, index.y+1, index.z+d)).values;
        q12 = getNodeByEulerMeshIndex(vector3u(index.x, index.y+1, index.z+d)).values;

        _x = node.coords.x = coords.x;
        _y = node.coords.y = coords.y;
        node.coords.z = _node.coords.z;

        node.setMaterialId(_node.getMaterialId());
        node.setRheologyMatrix(_node.getRheologyMatrix());
        node.number = _node.number;

        node.setCustomFlag(VIRT_CELL_AXIS_X_FLAG, 0);
        node.setCustomFlag(VIRT_CELL_AXIS_Y_FLAG, 0);
        node.setCustomFlag(VIRT_CELL_AXIS_Z_FLAG, d);
    }

    interpolateRectangle(x1, y1, x2, y2, _x, _y, q11, q12, q22, q21, node.values, VALUES_NUMBER);
    node.setIsBorder(true);

    return true;
}


bool EulerMesh::interpolateNode(CalcNode& origin, float dx, float dy, float dz, bool debug, CalcNode& targetNode, bool& isInnerPoint)
{
    if (!origin.getCustomFlag(VIRT_FLAG))
    {
	//Primarily we search for a second interpolation node	
       	float max = dx, min = dx;
    	int dmin = 0, dmax = 0, d = 0, s = 0;
    	if (dy > max) {max = dy; dmax = 1;};
    	if (dy < min) {min = dy; dmin = 1;};
    	if (dz > max) {max = dz; dmax = 2;};
    	if (dz < min) {min = dz; dmin = 2;};    

    	auto index = getCellEulerIndexByCoords(origin.coords);
    	vector3u dindex = vector3u(0, 0, 0);
    	if (fabs(max) > fabs(min))
    	{
	    s = 1;
    	    dindex[dmax] = 1;
            d = dmax;
    	}
    	else
    	{
 	    s = -1;
	    dindex[dmin] = -1;
	    d = dmin;
    	}
    	CalcNode second = getNodeByEulerMeshIndex(index+dindex);

	targetNode.setMaterialId(second.getMaterialId());

        vector3r dc = vector3r(dx, dy, dz);
        for (int i=0; i<3; i++) dc[i] *= (i == d);

        RheologyMatrixPtr   orm = origin.getRheologyMatrix(),
                            srm = second.getRheologyMatrix();
        gcm::real ao = 0, as = 0, bo = 0, bs = 0, ro = origin.getRho(), rs = second.getRho(), o1o = 0, o2o = 0, o3o = 0, o1s = 0, o2s = 0, o3s = 0, ao1 = 0, bo1 = 0;
        int n1o = 0, n2o = 0, n3o = 0, n1s = 0, n2s = 0, n3s = 0, n4o = 0, n5o = 0, n6o = 0;
	orm->decompose(origin, d);
	srm->decompose(second, d);
        if (s > 0)
        {
            for (int i=0; i<9; i++)
            {
                if (orm->getL(i, i) > ao) { ao = orm->getL(i, i); n1o = i;};
                if (orm->getL(i, i) < ao1) { ao1 = orm->getL(i, i); n4o = i;};
                if (srm->getL(i, i) < as) { as = srm->getL(i, i); n1s = i;};
            }
            for (int i=0; i<9; i++)
            {
                if (orm->getL(i, i) > bo && orm->getL(i, i) < ao) { bo = orm->getL(i, i); n2o = i;};
                if (orm->getL(i, i) < bo1 && orm->getL(i, i) > ao1) { bo1 = orm->getL(i, i); n5o = i;};
                if (srm->getL(i, i) < bs && srm->getL(i, i) > as) { bs = srm->getL(i, i); n2s = i;};
            }
            for (int i=0; i<9; i++)
            {
                if (i != n2o && fabs(bo - orm->getL(i, i)) < EQUALITY_TOLERANCE) n3o = i;
                if (i != n5o && fabs(bo1 - orm->getL(i, i)) < EQUALITY_TOLERANCE) n6o = i;
                if (i != n2s && fabs(bs - srm->getL(i, i)) < EQUALITY_TOLERANCE) n3s = i;
            }
            as *= -1; bs *= -1;
            if (n1o == n2o || n1o == n3o || n2o == n3o || fabs(orm->getMaxEigenvalue()) < EQUALITY_TOLERANCE || fabs(orm->getMinEigenvalue()) < EQUALITY_TOLERANCE)
                orm->decompose(origin, d);
            if (n1s == n2s || n1s == n3s || n2s == n3s || fabs(srm->getMaxEigenvalue()) < EQUALITY_TOLERANCE || fabs(srm->getMinEigenvalue()) < EQUALITY_TOLERANCE)
                srm->decompose(second, d);
        }
        else
        {
            for (int i=0; i<9; i++)
            {
                if (orm->getL(i, i) < ao) { ao = orm->getL(i, i); n1o = i;};
                if (orm->getL(i, i) > ao1) { ao1 = orm->getL(i, i); n4o = i;};
                if (srm->getL(i, i) > as) { as = srm->getL(i, i); n1s = i;};
            }
            for (int i=0; i<9; i++)
            {
                if (orm->getL(i, i) < bo && orm->getL(i, i) > ao) { bo = orm->getL(i, i); n2o = i;};
                if (orm->getL(i, i) > bo1 && orm->getL(i, i) < ao1) { bo1 = orm->getL(i, i); n5o = i;};
                if (srm->getL(i, i) > bs && srm->getL(i, i) < as) { bs = srm->getL(i, i); n2s = i;};
            }
            for (int i=0; i<9; i++)
            {
                if (i != n2o && fabs(bo - orm->getL(i, i)) < EQUALITY_TOLERANCE) n3o = i;
                if (i != n5o && fabs(bo1 - orm->getL(i, i)) < EQUALITY_TOLERANCE) n6o = i;
                if (i != n2s && fabs(bs - srm->getL(i, i)) < EQUALITY_TOLERANCE) n3s = i;
            }
            ao *= -1; bo *= -1;
            if (n1o == n2o || n1o == n3o || n2o == n3o|| fabs(orm->getMaxEigenvalue()) < EQUALITY_TOLERANCE || fabs(orm->getMinEigenvalue()) < EQUALITY_TOLERANCE)
                orm->decompose(origin, d);
            if (n1s == n2s || n1s == n3s || n2s == n3s || fabs(srm->getMaxEigenvalue()) < EQUALITY_TOLERANCE || fabs(srm->getMinEigenvalue()) < EQUALITY_TOLERANCE)
                srm->decompose(second, d);
        }
	
        targetNode.coords = origin.coords + dc;
        if (!outline.isInAABB(targetNode))
            return false;

	
	if (origin.getMaterialId() == second.getMaterialId())
	{ //whether the same, boring linear interpolation in target node
            isInnerPoint = interpolateNode(targetNode);
            return isInnerPoint;
	
	    //Enabling these two strings will result to border failure
            //interpolateSegment( origin.coords[d], second.coords[d], targetNode.coords[d], origin.values, second.values, targetNode.values, VALUES_NUMBER);
            //return true;
	}

            //we will need surroungind nodes, so we check materials before anything else
            CalcNode third = getNodeByEulerMeshIndex(index-dindex);
            CalcNode fourth = getNodeByEulerMeshIndex(index+dindex+dindex);

        if (true) //last desperate Chudov measure
        {
	    for (int i=0; i<9; i++)
		targetNode.values[i] = second.values[i];
	    isInnerPoint = true;
	    return true;
        }

            if (third.getMaterialId() != origin.getMaterialId())
            {   //it sucks, this is not normal situation, so we report it and try to make something from this crap
                //OMG, that's one twisted hack
                origin.setMaterialId(second.getMaterialId());
                return interpolateNode(origin, dx, dy, dz, debug, targetNode, isInnerPoint);
            }
            if (fourth.getMaterialId() != second.getMaterialId())
            {   //it sucks, this is not normal situation, so we report it and try to make something from this crap
                //OMG, that's one twisted hack
                origin.setMaterialId(second.getMaterialId());
                return interpolateNode(origin, dx, dy, dz, debug, targetNode, isInnerPoint);
            } 

//Attempt to replace interpolation with some sort of contact condition.--------------------------------------------------------
	if (origin.getMaterialId() != second.getMaterialId()) 
	{
	    //memory allocation
            gsl_matrix  *Go = gsl_matrix_alloc(6, 9),
			*Gs = gsl_matrix_alloc(6, 9),
                        *GU1o = gsl_matrix_alloc(6, 9),
			*GU1s = gsl_matrix_alloc(6, 9),
			*C = gsl_matrix_alloc(6, 6);
	    gsl_vector	*om = gsl_vector_alloc(6),
			*omO = gsl_vector_alloc(6),
			*omS = gsl_vector_alloc(6),
			*rhp = gsl_vector_alloc(6);

	    //initializing
	    gsl_matrix_set_zero(Go);
            gsl_matrix_set_zero(Gs);
            gsl_vector_set_zero(rhp);
            gsl_vector_set_zero(omO);
            gsl_vector_set_zero(omS);
	    gsl_matrix_set(Go, 0, 0, 1);
            gsl_matrix_set(Go, 1, 1, 1);
            gsl_matrix_set(Go, 2, 2, 1);
            gsl_matrix_set(Go, 3, 3, dindex[0]);
            gsl_matrix_set(Go, 3, 4, dindex[1]);
            gsl_matrix_set(Go, 3, 5, dindex[2]);
            gsl_matrix_set(Go, 4, 4, dindex[0]);
            gsl_matrix_set(Go, 4, 6, dindex[1]);
            gsl_matrix_set(Go, 4, 7, dindex[2]);
            gsl_matrix_set(Go, 5, 5, dindex[0]);
            gsl_matrix_set(Go, 5, 7, dindex[1]);
            gsl_matrix_set(Go, 5, 8, dindex[2]);
            gsl_matrix_set(Gs, 0, 0, 1);
            gsl_matrix_set(Gs, 1, 1, 1);
            gsl_matrix_set(Gs, 2, 2, 1);
            gsl_matrix_set(Gs, 3, 3, dindex[0]);
            gsl_matrix_set(Gs, 3, 4, dindex[1]);
            gsl_matrix_set(Gs, 3, 5, dindex[2]);
            gsl_matrix_set(Gs, 4, 4, dindex[0]);
            gsl_matrix_set(Gs, 4, 6, dindex[1]);
            gsl_matrix_set(Gs, 4, 7, dindex[2]);
            gsl_matrix_set(Gs, 5, 5, dindex[0]);
            gsl_matrix_set(Gs, 5, 7, dindex[1]);
            gsl_matrix_set(Gs, 5, 8, dindex[2]);

	    //calculating G*U1 for both sides
	    gcm::real res = 0;
	    for (int i=0; i<6; i++)
		for (int j=0; j<9; j++)
		{
		    res = 0;
		    for (int k=0; k<9; k++)
			res += gsl_matrix_get(Go, i, k)*orm->getU1(k, j);
		    gsl_matrix_set(GU1o, i, j, res);
		}
            for (int i=0; i<6; i++)
                for (int j=0; j<9; j++)
                {
                    res = 0;
                    for (int k=0; k<9; k++)
                        res += gsl_matrix_get(Gs, i, k)*srm->getU1(k, j);
                    gsl_matrix_set(GU1s, i, j, res);
                }
	    
	    //creating C matrix by taking necessary columns from G*U1
	    int noc[3] = {0}, nsc[3] = {0};
	    if (s > 0)
	    {
		int nc = 0;
		for (int j=0; j<9; j++)
		    if (orm->getL(j, j) < 0)
		    {
			for (int i=0; i<6; i++)
			    gsl_matrix_set(C, i, nc, gsl_matrix_get(GU1o, i, j));
			noc[nc] = j;
			nc++;
		    }
		if (nc != 3) {LOG_INFO("Not three negative lambdas in ORM" <<nc); return false;};
                for (int j=0; j<9; j++)
                    if (srm->getL(j, j) > 0)
                    {
                        for (int i=0; i<6; i++)
                            gsl_matrix_set(C, i, nc, -gsl_matrix_get(GU1s, i, j));
			nsc[nc] = j;
                        nc++;
                    }
                if (nc != 6) {LOG_INFO("Not three positive lambdas in SRM" <<nc); return false;};
	    }
	    else
            {
                int nc = 0;
                for (int j=0; j<9; j++)
                    if (orm->getL(j, j) > 0)
                    {
                        for (int i=0; i<6; i++)
                            gsl_matrix_set(C, i, nc, gsl_matrix_get(GU1o, i, j));
			noc[nc] = j;
                        nc++;
                    }
                if (nc != 3) {LOG_INFO("Not three positive lambdas in ORM" <<nc); return false;};
                for (int j=0; j<9; j++)
                    if (srm->getL(j, j) < 0)
                    {
                        for (int i=0; i<6; i++)
                            gsl_matrix_set(C, i, nc, -gsl_matrix_get(GU1s, i, j));
			nsc[nc] = j;
                        nc++;
                    }
                if (nc != 6) {LOG_INFO("Not three negative lambdas in SRM" <<nc); return false;};
            }

	    //counting rhp
	    int no[6] = {0}, ns[6] = {0};
	    gcm::real tau = getRecommendedTimeStep();
	    {	//that's what we can work with
		int nc = 0;
		gcm::real res = 0;
		for (int i=0; i<9; i++)
		{
		    if (fabs(orm->getL(i, i)) < EQUALITY_TOLERANCE)
		    {
			res = 0;
			for (int j=0; j<9; j++)
			    res += orm->getU(i, j)*origin.values[j];
			gsl_vector_set(omO, nc, res);
			no[nc] = i;
			nc++;
	 	    }
	
		    if (s > 0 && orm->getL(i, i) > 0)
                    {
                        interpolateSegment( third.coords[d], origin.coords[d], origin.coords[d] - tau*orm->getL(i, i), origin.values, third.values, targetNode.values, VALUES_NUMBER);
                        res = 0;
                        for (int j=0; j<9; j++)
                            res += orm->getU(i, j)*targetNode.values[j];
                        gsl_vector_set(omO, nc, res);
			no[nc] = i;
                        nc++;
                    }
		    else
		    if (s < 0 && orm->getL(i, i) < 0)
                    {
                        interpolateSegment( origin.coords[d], third.coords[d], origin.coords[d] - tau*orm->getL(i, i), origin.values, third.values, targetNode.values, VALUES_NUMBER);
                        res = 0;
                        for (int j=0; j<9; j++)
                            res += orm->getU(i, j)*targetNode.values[j];
                        gsl_vector_set(omO, nc, res);
			no[nc] = i;
                        nc++;
                    }		 
		}
                if (nc != 6) {LOG_INFO("Not enough lambdas in ORM 0 " <<nc); return false;};
		nc = 0;
                for (int i=0; i<9; i++)
		{
                    if (fabs(srm->getL(i, i)) < EQUALITY_TOLERANCE)
                    {
                        res = 0;
                        for (int j=0; j<9; j++)
                            res += srm->getU(i, j)*second.values[j];
                        gsl_vector_set(omS, nc, res);
			ns[nc] = i;
			nc++;
                    }
                    if (s > 0 && srm->getL(i, i) < 0)
                    {
                        interpolateSegment( second.coords[d], fourth.coords[d], second.coords[d] - tau*srm->getL(i, i), second.values, fourth.values, targetNode.values, VALUES_NUMBER);
                        res = 0;
                        for (int j=0; j<9; j++)
                            res += srm->getU(i, j)*targetNode.values[j];
                        gsl_vector_set(omS, nc, res);
			ns[nc] = i;
                        nc++;
                    }
                    else
                    if (s < 0 && srm->getL(i, i) > 0)
                    {
                        interpolateSegment( fourth.coords[d], second.coords[d], second.coords[d] - tau*srm->getL(i, i), second.values, fourth.values, targetNode.values, VALUES_NUMBER);
                        res = 0;
                        for (int j=0; j<9; j++)
                            res += srm->getU(i, j)*targetNode.values[j];
                        gsl_vector_set(omS, nc, res);
			ns[nc] = i;
                        nc++;
                    }

		}
                if (nc != 6) {LOG_INFO("Not enough lambdas in SRM" <<nc); return false;};
	    }		
	    //now we got omO and omS, can calculate rhp
	    for (int i=0; i<6; i++)
	    {
		res = 0;
		for (int j=0; j<6; j++)
		    res -= gsl_matrix_get(GU1o, i, no[j])*gsl_vector_get(omO, j);
                for (int j=0; j<6; j++)
                    res += gsl_matrix_get(GU1s, i, ns[j])*gsl_vector_get(omS, j);
		gsl_vector_set(rhp, i, res);
	    }

	    //solving C*om = rph system
 	    int s;
            gsl_permutation *p = gsl_permutation_alloc(6);
            gsl_permutation_init(p);
	    gsl_linalg_LU_decomp (C, p, &s);
	    gsl_linalg_LU_solve (C, p, rhp, om);	
	    gsl_permutation_free(p);
	    
	    //obtaining target node values from calculated omegas
	    for (int i=0; i<9; i++) targetNode.values[i] = 0;
	    for (int j=0; j<3; j++)
	    {
	   	for (int i=0; i<9; i++)
		    targetNode.values[i] += orm->getU1(i, noc[j])*gsl_vector_get(om, j);
	    }

	    //freeing the memory
	    gsl_matrix_free(Go);
            gsl_matrix_free(Gs);
            gsl_matrix_free(GU1o);
            gsl_matrix_free(GU1s);
            gsl_matrix_free(C);
            gsl_vector_free(om);    
            gsl_vector_free(omS);
            gsl_vector_free(omO);
            gsl_vector_free(rhp);
	    return true;
	}

//Formulas from Aki-Richards-----------------------------------------------------------------------------------------------------------
	if (false)
	{
	    for (int i=0; i<9; i++)
	    {
		o1o += orm->getU(n1o, i)*origin.values[i];
                o2o += orm->getU(n2o, i)*origin.values[i];
                o3o += orm->getU(n3o, i)*origin.values[i];
                o1s += srm->getU(n1s, i)*second.values[i];
                o2s += srm->getU(n2s, i)*second.values[i];
                o3s += srm->getU(n3s, i)*second.values[i];
	    }
	    gcm::real resP = 0, resS1 = 0, resS2 = 0;
	    resP = o1o*(rs*as - ro*ao)/(rs*as + ro*ao) + o1s*2*rs*as/(rs*as + ro*ao);
	    resS1 = o2o*(ro*bo - rs*bs)/(ro*bo + rs*bs) + o2s*2*rs*bs/(ro*bo + rs*bs);
            resS2 = o3o*(ro*bo - rs*bs)/(ro*bo + rs*bs) + o3s*2*rs*bs/(ro*bo + rs*bs);

	    for (int i=0; i<9; i++)
	    {
		targetNode.values[i] = orm->getU1(i, n4o)*resP + orm->getU1(i, n5o)*resS1 + orm->getU1(i, n6o)*resS2;
	    }
	    return true;
	}



//Riemann solution from Vlad ----------------------------------------------------------------------------------------------------------
	if (false)
	{
	    gsl_matrix  *R = gsl_matrix_alloc(9, 9),
			*S = gsl_matrix_alloc(9, 9),
			*R1 = gsl_matrix_alloc(9, 9);
	    //First we fill R with eigen vectors of corresponding indices:
	    //negative lambdas from left material (less coordinate), 
	    //zero (???) from origin,
	    //positive from right material (larger coordinate).
	    int n = 0;
	    for (int j=0; j<9; j++) 
	    {
		if (s < 0)
		{
		    if (orm->getL(j, j) < 0)
		    {
			for (int i=0; i<9; i++)
			{
			    gsl_matrix_set(R, n, i, orm->getU(j, i));
                            gsl_matrix_set(R1, i, n, orm->getU1(i, j));
			}
			n++;
		    }	
		}
                else {   
                    if (srm->getL(j, j) < 0)
                    {
                        for (int i=0; i<9; i++)
			{
                            gsl_matrix_set(R, n, i, srm->getU(j, i));
                            gsl_matrix_set(R1, i, n, srm->getU1(i, j));
			}
                        n++;
                    }
                }
	    }
	    if (n != 3) {LOG_INFO("Not 3 negative lambdas" << n); return false;};
 
            for (int j=0; j<9; j++)
            {
                if (orm->getL(j, j) == 0)
                {
                    for (int i=0; i<9; i++)
		    {
                        gsl_matrix_set(R, n, i, orm->getU(j, i));
                        gsl_matrix_set(R1, i, n, orm->getU1(i, j));
		    }
                    n++;
                }
            }
            if (n != 6) {LOG_INFO("Not 3 zero lambdas"); return false;};
            for (int j=0; j<9; j++)
            {
                if (s < 0)
                {
                    if (srm->getL(j, j) > 0)
                    {
                        for (int i=0; i<9; i++)
			{
                            gsl_matrix_set(R, n, i, srm->getU(j, i));
                            gsl_matrix_set(R1, i, n, srm->getU1(i, j));
			}
                        n++;
                    }
                }
                else {
                    if (orm->getL(j, j) > 0)
                    {
                        for (int i=0; i<9; i++)
			{
                            gsl_matrix_set(R, n, i, orm->getU(j, i));
                            gsl_matrix_set(R1, i, n, orm->getU1(i, j));
			}
                        n++;
                    }
                }
            }
            if (n != 9) {LOG_INFO("Not 3 pozitive lambdas"); return false;};

	    //Now we got R, R1 and all is left is S
	    gsl_matrix_set_identity(S);
	    gsl_matrix_set(S, 0, 0, -1);
            gsl_matrix_set(S, 1, 1, -1);
            gsl_matrix_set(S, 2, 2, -1);
            gsl_matrix_set(S, 3, 3, 0);
            gsl_matrix_set(S, 4, 4, 0);
            gsl_matrix_set(S, 5, 5, 0);

	    //When we have all the matrices, we can compute a Riemann solution
            gsl_matrix  *res0 = gsl_matrix_alloc(9, 9),
			*res1 = gsl_matrix_alloc(9, 9);
	    gsl_matrix_set_zero(res0);
            gsl_matrix_set_zero(res1);
	    float tmp;
	    for (int i=0; i<9; i++)
		for (int j=0; j<9; j++)
		{
		    tmp = 0;	
		    for (int k=0; k<9; k++)
			tmp += gsl_matrix_get(R1, i, k)*gsl_matrix_get(S, k, j);
		    gsl_matrix_set(res0, i, j, tmp);
		}
            for (int i=0; i<9; i++)
                for (int j=0; j<9; j++)
		{
		    tmp = 0;
                    for (int k=0; k<9; k++)
                        tmp += gsl_matrix_get(res0, i, k)*gsl_matrix_get(R, k, j);
		    gsl_matrix_set(res1, i, j, tmp);
		}
	    gcm::real res2[9];
            for (int i=0; i<9; i++)
	    {
		res2[i] = 0;
                for (int j=0; j<9; j++)
		    res2[i] += gsl_matrix_get(res1, i, j)*s*(origin.values[j] - second.values[j])/2.0;
	    	targetNode.values[i] = (origin.values[i] + second.values[i])/2.0 + res2[i];
	    }
	    gsl_matrix_free(res0);
	    gsl_matrix_free(res1);
	    gsl_matrix_free(R);
            gsl_matrix_free(S);
            gsl_matrix_free(R1);
	    return true;
	}
    } else
    {
	targetNode.coords = origin.coords+vector3r(dx, dy, dz);
        assert_true(
            (dx == 0.0 && dy == 0.0) ||
            (dz == 0.0 && dy == 0.0) ||
            (dx == 0.0 && dz == 0.0)
        );

        vector3u index;
        bool inner = getNodeEulerMeshIndex(origin, index);
        assert_true(inner);
        auto d1 = origin.getCustomFlag(VIRT_CELL_AXIS_X_FLAG);
        auto d2 = origin.getCustomFlag(VIRT_CELL_AXIS_Y_FLAG);
        auto d3 = origin.getCustomFlag(VIRT_CELL_AXIS_Z_FLAG);
        index.x -= d1;
        index.y -= d2;
        index.z -= d3;
        assert_true(cellStatus[index.x][index.y][index.z]);
        if (dx != 0.0)
        {
            isInnerPoint = ((d1 == 1 && dx < 0.0) || (d1 == 0 && dx > 0.0));
        } else if (dy != 0.0)
        {
            isInnerPoint = ((d2 == 1 && dy < 0.0) || (d2 == 0 && dy > 0.0));
        } else
        {
            isInnerPoint = ((d3 == 1 && dz < 0.0) || (d3 == 0 && dz > 0.0));
        }
        if (isInnerPoint)
        {
            auto res = interpolateNode(targetNode, index);
            assert_true(res);
            return true;
        }
        else
            return false;
    }
}

bool EulerMesh::getNodeEulerMeshIndex(const CalcNode& node, vector3u& indexes) const
{
    const auto& c1 = const_cast<EulerMesh*>(this)->getNodeByEulerMeshIndex(vector3u(0, 0, 0)).coords;
    const auto& c2 = const_cast<EulerMesh*>(this)->getNodeByEulerMeshIndex(nodeDimensions-vector3u(1, 1, 1)).coords;

    if (node.coords < c1 || node.coords > c2)
        return false;

    auto localNumber = Mesh::getNodeLocalIndex(node.number);

    uint t = localNumber % (nodeDimensions.y*nodeDimensions.z);

    indexes.x = localNumber / (nodeDimensions.y*nodeDimensions.z);
    indexes.y = t / nodeDimensions.z;
    indexes.z = t % nodeDimensions.z;

    return true;
}

int EulerMesh::getNodeLocalIndexByEulerMeshIndex(const vector3u& index) const
{
    return index.z + index.y*nodeDimensions.z + index.x*nodeDimensions.y*nodeDimensions.z;
}

CalcNode& EulerMesh::getNodeByEulerMeshIndex(const vector3u& index) {
    return Mesh::getNodeByLocalIndex(getNodeLocalIndexByEulerMeshIndex(index));
}

void EulerMesh::setCellSize(const vector3r& cellSize)
{
    assert_gt(cellSize.x, 0);
    assert_gt(cellSize.y, 0);
    assert_gt(cellSize.z, 0);

    this->cellSize = cellSize;
}

void EulerMesh::setDimensions(const vector3u& dimensions)
{
    assert_gt(dimensions.x, 0);
    assert_gt(dimensions.y, 0);
    assert_gt(dimensions.z, 0);

    this->dimensions = dimensions;
    nodeDimensions = dimensions + vector3u(1, 1, 1);
}

void EulerMesh::setCenter(const vector3r& center)
{
    this->center = center;
}

void EulerMesh::generateMesh() {
    // generate mesh
    auto halfSize = cellSize%dimensions/2;
    auto point = center-halfSize;

    LOG_DEBUG("Generating Euler mesh with AABB: [" << point << ", " << point + 2*halfSize << "]");
    LOG_DEBUG("Mesh center: " << center);
    LOG_DEBUG("Mesh dimensions: " << dimensions);
    LOG_DEBUG("Mesh element size: " << cellSize);

    uint nodesNumber = (dimensions.x+1)*(dimensions.y+1)*(dimensions.z+1);
    LOG_DEBUG("Total number of node: " << nodesNumber);
    nodes.reserve(nodesNumber);

    CalcNode node;
    node.setPlacement(true);
    // FIXME do we actually need this?
    node.setUsed(false);
    int index = 0;
    for (uint i = 0; i <= dimensions.x; i++)
        for (uint j = 0; j <= dimensions.y; j++)
            for (uint k = 0; k <= dimensions.z; k++)
            {
                node.coords = point + cellSize%vector3u(i, j, k);
                node.number = index++;
                addNode(node);
            }

    cellStatus = new uchar**[dimensions.x];
    for (uint i = 0; i < dimensions.x; i++)
    {
        cellStatus[i] = new uchar*[dimensions.y];
        for (uint j = 0; j < dimensions.y; j++)
            cellStatus[i][j] = new uchar[dimensions.z];
    }

}

vector3u EulerMesh::getCellEulerIndexByCoords(const vector3r& coords) const
{
    vector3i index = getCellEulerIndexByCoordsUnsafe(coords);

    assert_ge(index.x, 0);
    assert_ge(index.y, 0);
    assert_ge(index.z, 0);

    assert_lt(index.x, dimensions.x);
    assert_lt(index.y, dimensions.y);
    assert_lt(index.z, dimensions.z);

    return vector3u(index.x, index.y, index.z);
}

vector3r EulerMesh::getCellCenter(const vector3u& index) const {
    assert_lt(index.x, dimensions.x);
    assert_lt(index.y, dimensions.y);
    assert_lt(index.z, dimensions.z);

    return const_cast<EulerMesh*>(this)->getNodeByEulerMeshIndex(index).coords+cellSize/2;
}

uint EulerMesh::getCellLocalIndexByEulerIndex(const vector3u& index) const
{
    return index.z + index.y*dimensions.z + index.x*dimensions.y*dimensions.z;
}


const vector3r& EulerMesh::getCellSize() const
{
    return cellSize;
}

const vector3u& EulerMesh::getDimensions() const
{
    return dimensions;
}

const vector3u& EulerMesh::getNodeDimensions() const
{
    return nodeDimensions;
}

vector3i EulerMesh::getCellEulerIndexByCoordsUnsafe(const vector3r& coords) const
{
    const vector3r& p = const_cast<EulerMesh*>(this)->getNodeByEulerMeshIndex(vector3u(0, 0, 0)).coords;

    vector3r index = (coords-p)/cellSize;

    return vector3i(floor(index.x), floor(index.y), floor(index.z));
}

uchar EulerMesh::getCellStatus(const vector3u& index) const
{
    assert_lt(index.x, dimensions.x);
    assert_lt(index.y, dimensions.y);
    assert_lt(index.z, dimensions.z);

    return cellStatus[index.x][index.y][index.z];
}

bool EulerMesh::interpolateNode(CalcNode& node) {
	if (!outline.isInAABB(node))
		return false;
    auto index = getCellEulerIndexByCoords(node.coords);

    return interpolateNode(node, index);
}

bool EulerMesh::interpolateNode(CalcNode& node, const vector3u &index) {
    if (!cellStatus[index.x][index.y][index.z])
        return false;

    auto& n000 = getNodeByEulerMeshIndex(index);
    auto& n010 = getNodeByEulerMeshIndex(index+vector3u(0, 1, 0));
    auto& n110 = getNodeByEulerMeshIndex(index+vector3u(1, 1, 0));
    auto& n100 = getNodeByEulerMeshIndex(index+vector3u(1, 0, 0));
    auto& n001 = getNodeByEulerMeshIndex(index+vector3u(0, 0, 1));
    auto& n011 = getNodeByEulerMeshIndex(index+vector3u(0, 1, 1));
    auto& n111 = getNodeByEulerMeshIndex(index+vector3u(1, 1, 1));
    auto& n101 = getNodeByEulerMeshIndex(index+vector3u(1, 0, 1));

    interpolateBox(n000.coords.x, n000.coords.y, n000.coords.z, n111.coords.x, n111.coords.y, n111.coords.z, node.coords.x, node.coords.y, node.coords.z, n000.values, n001.values, n010.values, n011.values, n100.values, n101.values, n110.values, n111.values, node.values, VALUES_NUMBER);
    node.setMaterialId(n000.getMaterialId());
    node.setRheologyMatrix(n000.getRheologyMatrix());

    return true;
}
