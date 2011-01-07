#ifndef _GCM_BASIC_TETR_MESH_H
#define _GCM_BASIC_TETR_MESH_H  1

#include "Mesh.h"
#include "../system/quick_math.h"

class BasicTetrMesh : public Mesh
{
public:
	BasicTetrMesh();
	~BasicTetrMesh();
	int load_mesh(char* file_name);

	vector<ElasticNode> nodes;

private:
	void do_next_part_step(float tau, int stage);
	void move_coords(float tau);
	float get_max_possible_tau();
	float get_min_h();
	bool point_in_tetr(float x, float y, float z, Tetrahedron_1st_order* tetr);
	Tetrahedron_1st_order* find_owner_tetr(float x, float y, float z, ElasticNode* node);
	int interpolate(ElasticNode* node, Tetrahedron_1st_order* tetr);

	vector<Tetrahedron_1st_order> tetrs;
	vector<ElasticNode> new_nodes;

	quick_math qm_engine;
};

#include "BasicTetrMesh.inl"

#endif
