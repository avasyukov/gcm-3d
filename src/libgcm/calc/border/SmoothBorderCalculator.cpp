#include "libgcm/calc/border/SmoothBorderCalculator.hpp"

#include "libgcm/node/CalcNode.hpp"

SmoothBorderCalculator::SmoothBorderCalculator()
{
    INIT_LOGGER( "gcm.SmoothBorderCalculator" );
};

SmoothBorderCalculator::~SmoothBorderCalculator()
{
};

void SmoothBorderCalculator::doCalc(CalcNode& cur_node, CalcNode& new_node, RheologyMatrix3D& matrix,
                            vector<CalcNode>& previousNodes, bool inner[],
                            float outer_normal[], float scale)
{
    assert(previousNodes.size() == 9);

    LOG_TRACE("Starting calc with SmoothBorderCalculator");

    int inner_node_num = -1;
    int outer_count = 0;
    for(int i = 0; i < 9; i++)
        if( inner[i] )
            inner_node_num = i;
        else
            outer_count++;

    assert( outer_count != 3 );

    for(int j = 0; j < 9; j++)
        new_node.values[j] = previousNodes[inner_node_num].values[j];

    LOG_TRACE("Calc done");
};
