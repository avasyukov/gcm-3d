#ifndef _GCM_TETR_MESH_1ST_ORDER_H
#define _GCM_TETR_MESH_1ST_ORDER_H  1

#include "TetrMesh.h"
#include "../system/quick_math.h"
#include "../datatypes/ElasticNode.h"
#include "../datatypes/Tetrahedron_1st_order.h"
#include "../methods/GCM_Tetr_Elastic_Interpolation_1stOrder.h"
#include "../rheotypes/VoidRheologyCalculator.h"

class TetrMesh_1stOrder : public TetrMesh
{
public:
	TetrMesh_1stOrder();
	~TetrMesh_1stOrder();
	int load_mesh(char* file_name);
	Tetrahedron_1st_order* find_owner_tetr(float x, float y, float z, ElasticNode* node);
	int interpolate(ElasticNode* node, Tetrahedron* tetr);
	int do_next_step();
	float get_max_possible_tau();

	vector<ElasticNode> nodes;

private:
	int do_next_part_step(float tau, int stage);
	void move_coords(float tau);
	int set_stress(float tau);
	int proceed_rheology();

	float get_min_h();
	bool point_in_tetr(float x, float y, float z, Tetrahedron_1st_order* tetr);

	vector<Tetrahedron_1st_order> tetrs;
	vector<ElasticNode> new_nodes;

	quick_math qm_engine;
};

#include "TetrMesh_1stOrder.inl"

#endif
