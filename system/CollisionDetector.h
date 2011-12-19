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
	int find_collisions(TetrMesh_1stOrder* mesh1, TetrMesh_1stOrder* mesh2, vector<ElasticNode>* virt_nodes, float time_step);
protected:
	void find_elements_in_intersect(TetrMesh_1stOrder* mesh, MeshOutline* intersect, vector<int>* nodes_vector, vector<int>* tetrs_vector);
	bool node_in_intersection(ElasticNode* node, MeshOutline* intersect);
	int process_mesh(vector<int>* nodes_vector, TetrMesh_1stOrder* current_mesh, vector<int>* tetrs_vector, TetrMesh_1stOrder* other_mesh, float time_step, vector<ElasticNode>* virt_nodes);
	int process_direction(ElasticNode* _node, float move, int axis_num, vector<int>* tetrs_vector, TetrMesh_1stOrder* tetrs_mesh, vector<ElasticNode>* virt_nodes);
	Logger* logger;
	float treshold;
	vector<int> mesh1_nodes;
	vector<int> mesh2_nodes;
	vector<int> mesh1_tetrs;
	vector<int> mesh2_tetrs;
};

#include "CollisionDetector.inl"

#endif
