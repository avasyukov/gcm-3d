#include "libgcm/calc/border/ExternalForceNaiveCalculator.hpp"

#include <boost/lexical_cast.hpp>
#include "libgcm/node/CalcNode.hpp"

using boost::lexical_cast;

using namespace gcm;
using std::vector;

ExternalForceNaiveCalculator::ExternalForceNaiveCalculator()
{
};

ExternalForceNaiveCalculator::~ExternalForceNaiveCalculator()
{
};

void ExternalForceNaiveCalculator::setParameters(const xml::Node& params)
{
    real normalStress = lexical_cast<real>(params.getAttributeByName("normalStress", "0.0"));
    real tangentialStress = lexical_cast<real>(params.getAttributeByName("tangentialStress", "0.0"));
    real tangentialDirection[3];
    if(tangentialStress == 0.0)
    {
        tangentialDirection[0] = lexical_cast<real>(params.getAttributeByName("tangentialX", "1.0"));
        tangentialDirection[1] = lexical_cast<real>(params.getAttributeByName("tangentialY", "0.0"));
        tangentialDirection[2] = lexical_cast<real>(params.getAttributeByName("tangentialZ", "0.0"));
    }
    else
    {
        tangentialDirection[0] = lexical_cast<real>(params["tangentialX"]);
        tangentialDirection[1] = lexical_cast<real>(params["tangentialY"]);
        tangentialDirection[2] = lexical_cast<real>(params["tangentialZ"]);
    }
    
    normal_stress = normalStress;
    tangential_stress = tangentialStress;
    real dtmp = vectorNorm(tangentialDirection[0], tangentialDirection[1], tangentialDirection[2]);
    tangential_direction[0] = tangentialDirection[0] / dtmp;
    tangential_direction[1] = tangentialDirection[1] / dtmp;
    tangential_direction[2] = tangentialDirection[2] / dtmp;
};

void ExternalForceNaiveCalculator::doCalc(CalcNode& cur_node, CalcNode& new_node, RheologyMatrixPtr matrix,
                            vector<CalcNode>& previousNodes, bool inner[],
                            float outer_normal[], float scale)
{
    real force[3] = {   tangential_stress * tangential_direction[0] + normal_stress * outer_normal[0], 
                        tangential_stress * tangential_direction[1] + normal_stress * outer_normal[1], 
                        tangential_stress * tangential_direction[2] + normal_stress * outer_normal[2] };
    new_node.vx = previousNodes[0].vx;
    new_node.vy = previousNodes[0].vy;
    new_node.vz = previousNodes[0].vz;
    new_node.sxx = scale * (force[0] * outer_normal[0]);
    new_node.sxy = scale * (force[1] * outer_normal[0] + force[0] * outer_normal[1]);
    new_node.sxz = scale * (force[2] * outer_normal[0] + force[0] * outer_normal[2]);
    new_node.syy = scale * (force[1] * outer_normal[1]);
    new_node.syz = scale * (force[1] * outer_normal[2] + force[2] * outer_normal[1]);
    new_node.szz = scale * (force[2] * outer_normal[2]);
};
