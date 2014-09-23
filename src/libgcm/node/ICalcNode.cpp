#include "libgcm/node/ICalcNode.hpp"

using namespace gcm;

ICalcNode::ICalcNode(int num, float x, float y, float z): Node(num, x, y, z)
{

}

ICalcNode::ICalcNode(const ICalcNode& src) {
    *this = src;
}
