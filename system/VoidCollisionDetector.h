#ifndef _GCM_VOID_COLLISION_DETECTOR
#define _GCM_VOID_COLLISION_DETECTOR 1

#include <vector>
#include <string>

using std::vector;
using std::string;

class VoidCollisionDetector;

#include "CollisionDetector.h"

class VoidCollisionDetector : public CollisionDetector
{
public:
	VoidCollisionDetector();
	~VoidCollisionDetector();
	void find_collisions(vector<ElasticNode> &virt_nodes);
protected:
};

#endif
