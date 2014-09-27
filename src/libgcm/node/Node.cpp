#include "libgcm/node/Node.hpp"

using namespace gcm;

Node::Node() : Node(-1)
{
}

Node::Node(int num) : Node(num, vector3r())
{
}

Node::Node(int num, const vector3r& coords) : number(num), coords(coords)
{
}

Node::Node(const Node& src) {
    *this = src;
}

Node::~Node()
{
}
