#include "libgcm/node/ICalcNode.hpp"

gcm::ICalcNode::ICalcNode(int num, float x, float y, float z): Node(num, x, y, z)
{

}

gcm::ICalcNode::ICalcNode(const ICalcNode& src) {
    *this = src;
}
