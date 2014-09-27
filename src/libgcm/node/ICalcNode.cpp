#include "libgcm/node/ICalcNode.hpp"

using namespace gcm;

ICalcNode::ICalcNode(int num, const vector3r& coords): Node(num, coords)
{

}

ICalcNode::ICalcNode(const ICalcNode& src) {
    *this = src;
}
