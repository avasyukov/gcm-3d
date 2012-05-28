#ifndef _GCM_COLLISION_DETECTOR_FOR_LAYERS
#define _GCM_COLLISION_DETECTOR_FOR_LAYERS 1

#include <vector>
#include <string>

using std::vector;
using std::string;

class CollisionDetectorForLayers;

#include "CollisionDetector.h"

class CollisionDetectorForLayers : public CollisionDetector
{
public:
	CollisionDetectorForLayers();
	~CollisionDetectorForLayers();
	void find_collisions(vector<ElasticNode> &virt_nodes);
protected:
private:
	void find_index_shift(TetrMesh_1stOrder* mesh1, TetrMesh_1stOrder* mesh2);
	void process_mesh(vector<int> &nodes_vector, TetrMesh_1stOrder* current_mesh, TetrMesh_1stOrder* other_mesh, 
					vector<ElasticNode> &virt_nodes);
	void add_node_by_local_num(TetrMesh_1stOrder* other_mesh, int local_num, vector<ElasticNode> &virt_nodes);
	int index_shift;
	int shift_direction;
};

#endif
