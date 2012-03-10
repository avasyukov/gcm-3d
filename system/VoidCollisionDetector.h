#ifndef _GCM_VOID_COLLISION_DETECTOR
#define _GCM_VOID_COLLISION_DETECTOR 1

#include <vector>
#include <string>

using std::vector;
using std::string;

class VoidCollisionDetector;

#include "CollisionDetector.h"
#include "../datatypes/ElasticNode.h"
#include "../meshtypes/TetrMesh_1stOrder.h"
#include "Logger.h"

class VoidCollisionDetector : public CollisionDetector
{
public:
	VoidCollisionDetector();
	~VoidCollisionDetector();
	void set_treshold(float value);
	void attach(Logger* new_logger);
	int find_collisions(TetrMesh_1stOrder* mesh1, TetrMesh_1stOrder* mesh2, vector<ElasticNode>* virt_nodes, float time_step);
protected:
	Logger* logger;
	float treshold;
};

#endif
