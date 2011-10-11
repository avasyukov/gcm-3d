#ifndef _GCM_NODE_H
#define _GCM_NODE_H  1

#include <string>
#include <vector>

using std::string;
using std::vector;

#define LOCAL 1
#define REMOTE 0
#define UNUSED -1

class Node
{
public:
	Node();
	~Node();
	int zone_num;
	int local_num;
	int remote_num;
	int absolute_num;
	int placement_type; // We need this to store NOT only LOCAL / REMOTE state. We have UNUSED flag also.
	float coords[3];
	float fixed_coords[3];
protected:
private:
};

#include "Node.inl"

#endif
