#ifndef _GCM_COLLISION_DETECTOR_FOR_LAYERS
#define _GCM_COLLISION_DETECTOR_FOR_LAYERS 1

#include <vector>
#include <string>

using std::vector;
using std::string;

class CollisionDetectorForLayers;

#include "CollisionDetector.h"
#include "../datatypes/MeshOutline.h"

class CollisionDetectorForLayers : public CollisionDetector
{
public:
	CollisionDetectorForLayers();
	~CollisionDetectorForLayers();
	void set_treshold(float value);
	void attach(Logger* new_logger);
	int find_collisions(TetrMesh_1stOrder* mesh1, TetrMesh_1stOrder* mesh2, vector<ElasticNode>* virt_nodes, float time_step);

protected:
	void find_index_shift(TetrMesh_1stOrder* mesh1, TetrMesh_1stOrder* mesh2);
	void find_nodes_in_intersect(TetrMesh_1stOrder* mesh, MeshOutline* intersect, vector<int>* nodes_vector);
	bool node_in_intersection(ElasticNode* node, MeshOutline* intersect);
	void process_mesh(vector<int>* nodes_vector, TetrMesh_1stOrder* current_mesh, TetrMesh_1stOrder* other_mesh, vector<ElasticNode>* virt_nodes);

	int shift_direction;
	float index_shift;
	vector<int> mesh1_nodes;
	vector<int> mesh2_nodes;
};

#endif
