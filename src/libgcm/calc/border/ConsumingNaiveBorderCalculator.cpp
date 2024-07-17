#include "libgcm/calc/border/ConsumingNaiveBorderCalculator.hpp"

#include "libgcm/node/CalcNode.hpp"

using namespace gcm;
using std::vector;

ConsumingNaiveBorderCalculator::ConsumingNaiveBorderCalculator()
{
    INIT_LOGGER( "gcm.ConsumingNaiveBorderCalculator" );
};

ConsumingNaiveBorderCalculator::~ConsumingNaiveBorderCalculator()
{
};

void ConsumingNaiveBorderCalculator::setParameters(const xml::Node& params)
{
    
};

void ConsumingNaiveBorderCalculator::doCalc(CalcNode& cur_node, CalcNode& new_node, RheologyMatrixPtr matrix,
                            vector<CalcNode>& previousNodes, bool inner[],
                            float outer_normal[], float scale)
{
        for (auto &u : new_node.values)
            u = 0.0;
};
