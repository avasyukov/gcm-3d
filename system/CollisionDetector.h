#ifndef _GCM_COLLISION_DETECTOR
#define _GCM_COLLISION_DETECTOR 1

#include <vector>
#include <string>

#include "../datatypes/MeshOutline.h"

using std::string;
using std::vector;

class CollisionDetector
{
public:
	CollisionDetector();
	~CollisionDetector();
	void set_treshold(float value);
	void attach(Logger* new_logger);
	int find_collisions(TetrMesh_1stOrder* mesh1, TetrMesh_1stOrder* mesh2);
protected:
	bool node_in_intersection(ElasticNode* node, MeshOutline* intersect);
	Logger* logger;
	float treshold;
	vector<int> mesh1_nodes;
	vector<int> mesh2_nodes;
	vector<int> mesh1_tetrs;
	vector<int> mesh2_tetrs;
};

#include "CollisionDetector.inl"

#endif
