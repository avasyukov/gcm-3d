#include "libgcm/node/Node.hpp"

using namespace gcm;

Node::Node() : Node(-1)
{
}

Node::Node(int num) : Node(num, 0.0, 0.0, 0.0)
{
}

Node::Node(int num, float x, float y, float z) : number(num), x(x), y(y), z(z)
{
}

Node::Node(const Node& src) {
    *this = src;
}

Node::~Node()
{
}
