#include "node/Node.h"

gcm::Node::Node() : Node(-1)
{
}

gcm::Node::Node(int num) : Node(num, 0.0, 0.0, 0.0)
{
}

gcm::Node::Node(int num, float x, float y, float z) : number(num), x(x), y(y)
{
}

gcm::Node::~Node()
{
}
