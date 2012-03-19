#ifndef _GCM_PARALLEL_COLLISION_DETECTOR
#define _GCM_PARALLEL_COLLISION_DETECTOR 1

#include <vector>
#include <string>

using std::string;
using std::vector;

class ParallelCollisionDetector;

#include "CollisionDetector.h"
#include "DataBus.h"
#include "../datatypes/MeshOutline.h"
#include "../datatypes/Triangle.h"
#include "../datatypes/ElasticNode.h"

class ParallelCollisionDetector: public CollisionDetector
{
public:
	ParallelCollisionDetector();
	~ParallelCollisionDetector();
	void attach(Logger* new_logger);
	void find_collisions(vector<ElasticNode> &nodes, vector<ElasticNode> &border_nodes, vector<Triangle> &border_faces) ;
protected:
	Logger *logger;
};
#endif
