#include "libgcm/calc/border/FreeNaiveBorderCalculator.hpp"

#include "libgcm/node/CalcNode.hpp"

using namespace gcm;
using std::vector;

FreeNaiveBorderCalculator::FreeNaiveBorderCalculator()
{
    INIT_LOGGER( "gcm.FreeNaiveBorderCalculator" );
};

FreeNaiveBorderCalculator::~FreeNaiveBorderCalculator()
{
};

void FreeNaiveBorderCalculator::setParameters(const xml::Node& params)
{
    
};

void FreeNaiveBorderCalculator::doCalc(CalcNode& cur_node, CalcNode& new_node, RheologyMatrixPtr matrix,
                            vector<CalcNode>& previousNodes, bool inner[],
                            float outer_normal[], float scale)
{
    new_node.vx = previousNodes[0].vx;
    new_node.vy = previousNodes[0].vy;
    new_node.vz = previousNodes[0].vz;
    new_node.sxx = 0; //- previousNodes[0].sxx;
    new_node.sxy = 0; //- previousNodes[0].sxy;
    new_node.sxz = 0; //- previousNodes[0].sxz;
    new_node.syy = 0; //- previousNodes[0].syy;
    new_node.syz = 0; //- previousNodes[0].syz;
    new_node.szz = 0; //- previousNodes[0].szz;
};
