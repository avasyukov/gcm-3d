#include "Node.h"

gcm::Node::Node() {
	//std::cout << "Empty constructor called\n";
	number = -1;
	for( int i = 0; i < 3; i++ )
		coords[i] = 0;
}

gcm::Node::Node(int _num) {
	//std::cout << "Partial constructor called\n";
	Node();
	number = _num;
}

gcm::Node::Node(int _num, float _x, float _y, float _z) {
	//std::cout << "Complete constructor called\n";
	Node();
	number = _num;
	x = _x;
	y = _y;
	z = _z;
}

gcm::Node::~Node() {
	
}