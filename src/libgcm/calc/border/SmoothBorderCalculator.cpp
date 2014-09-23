#include "libgcm/calc/border/SmoothBorderCalculator.hpp"

#include "libgcm/node/CalcNode.hpp"

using namespace gcm;
using std::vector;

SmoothBorderCalculator::SmoothBorderCalculator()
{
    INIT_LOGGER( "gcm.SmoothBorderCalculator" );
};

SmoothBorderCalculator::~SmoothBorderCalculator()
{
};

void SmoothBorderCalculator::setParameters(const xml::Node& params)
{
    
};

void SmoothBorderCalculator::doCalc(CalcNode& cur_node, CalcNode& new_node, RheologyMatrixPtr matrix,
                            vector<CalcNode>& previousNodes, bool inner[],
                            float outer_normal[], float scale)
{
    assert_eq(previousNodes.size(), 9);

    LOG_TRACE("Starting calc with SmoothBorderCalculator");

    int inner_node_num = -1;
    int outer_count = 0;
    for(int i = 0; i < 9; i++)
        if( inner[i] )
            inner_node_num = i;
        else
            outer_count++;

    assert_ne(outer_count, 3 );

    for(int j = 0; j < 9; j++)
        new_node.values[j] = previousNodes[inner_node_num].values[j];

    LOG_TRACE("Calc done");
};
