#ifndef _GCM_BRUTEFORCE_COLLISION_DETECTOR
#define _GCM_BRUTEFORCE_COLLISION_DETECTOR 1

#include <vector>
#include <string>

using std::vector;
using std::string;

class BruteforceCollisionDetector;

#include "CollisionDetector.h"

class BruteforceCollisionDetector : public CollisionDetector
{
public:
	BruteforceCollisionDetector();
	~BruteforceCollisionDetector();
	void find_collisions(vector<ElasticNode> &virt_nodes);
protected:
};

#endif
