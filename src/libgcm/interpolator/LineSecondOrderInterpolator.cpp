#include "libgcm/interpolator/LineSecondOrderInterpolator.hpp"

#include "libgcm/node/CalcNode.hpp"

using namespace gcm;

LineSecondOrderInterpolator::LineSecondOrderInterpolator()
{
    type = "LineSecondOrderInterpolator";
    INIT_LOGGER("gcm.LineSecondOrderInterpolator");
}

LineSecondOrderInterpolator::~LineSecondOrderInterpolator()
{
}

void LineSecondOrderInterpolator::interpolate(CalcNode& node, CalcNode& nodeLeft, CalcNode& nodeCur, CalcNode& nodeRight)
{
    LOG_TRACE("Start interpolation");

    float lenLeft = distance(nodeCur.coords, nodeLeft.coords);
    float lenRight = distance(nodeCur.coords, nodeRight.coords);
    if( fabs(lenLeft - lenRight) > EQUALITY_TOLERANCE * (lenLeft + lenRight) * 0.5 )
        THROW_BAD_MESH("LineSecondOrderInterpolator can work on uniform grid only");
    
    float h = (lenLeft + lenRight) * 0.5;
    // FIXME_ASAP
    float x = (node.x - nodeCur.x) + (node.y - nodeCur.y) + (node.z - nodeCur.z);

    for (int i = 0; i < 9; i++) {
        float rVal = nodeRight.values[i];
        float lVal = nodeLeft.values[i];
        float cVal = nodeCur.values[i];
        float a = (rVal - 2 * cVal + lVal) / (2 * h * h);
        float b = (rVal - lVal) / (2 * h);
        float c = cVal;
        node.values[i] = a * x * x + b * x + c;
        float max = ( rVal > cVal ? (rVal > lVal ? rVal : lVal) : (cVal > lVal ? cVal : lVal) );
        float min = ( rVal < cVal ? (rVal < lVal ? rVal : lVal) : (cVal < lVal ? cVal : lVal) );
        if(node.values[i] > max)
        {
            node.values[i] = max;
        }
        if(node.values[i] < min)
        {
            node.values[i] = min;
        }
    }

    // FIXME_ASAP
    node.setRho(nodeCur.getRho());
    node.setMaterialId(nodeCur.getMaterialId());

    LOG_TRACE("Interpolation done");
}
