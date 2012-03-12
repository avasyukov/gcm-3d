#ifndef _GCM_COLLISION_DETECTOR
#define _GCM_COLLISION_DETECTOR 1

#include <vector>
#include <string>

using std::vector;
using std::string;

class CollisionDetector;

#include "../datatypes/ElasticNode.h"
#include "../meshtypes/TetrMesh_1stOrder.h"
#include "Logger.h"
#include "GCMException.h"

class CollisionDetector
{
public:
	CollisionDetector();
	~CollisionDetector();
	void set_treshold(float value);
	void attach(Logger* new_logger);
	virtual int find_collisions(TetrMesh_1stOrder* mesh1, TetrMesh_1stOrder* mesh2, vector<ElasticNode>* virt_nodes, float time_step) = 0;
protected:
	Logger* logger;
	float treshold;
};

#endif
