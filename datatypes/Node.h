#ifndef _GCM_NODE_H
#define _GCM_NODE_H  1

#include <string>
#include <vector>

using std::string;
using std::vector;

class Node
{
public:
	Node();
	~Node();
	int zone_num;
	int local_num;
	int remote_num;
	int absolute_num;
	float coords[3];
	float fixed_coords[3];
protected:
private:
};

#include "Node.inl"

#endif
