#ifndef _GCM_TETR_MESH_1ST_ORDER_H
#define _GCM_TETR_MESH_1ST_ORDER_H  1

#include <sstream>
#include <algorithm>

#include "TetrMesh.h"
#include "../system/quick_math.h"
#include "../datatypes/ElasticNode.h"
#include "../datatypes/Tetrahedron_1st_order.h"
#include "../methods/GCM_Tetr_Elastic_Interpolation_1stOrder.h"
#include "../rheotypes/VoidRheologyCalculator.h"

using std::stringstream;
using std::sort;
using std::unique;
using std::ios;

struct TetrMesh_1stOrder_stats
{
	int find_owner_tetr_quick_searches;
	int find_owner_tetr_long_searches;
};

//#define DEBUG_MESH_GEOMETRY 1

class TetrMesh_1stOrder : public TetrMesh
{
public:
	TetrMesh_1stOrder();
	~TetrMesh_1stOrder();

	int load_msh_file(char* file_name);
        int load_node_ele_files(char* node_file_name, char* ele_file_name);
	int load_gmv_file(char* file_name);

	int pre_process_mesh(float scale_factor);

//	bool point_in_tetr(float ext_x, float ext_y, float ext_z, Tetrahedron* tetr); __attribute__((optimize("O1")));
//	bool point_in_tetr(float x, float y, float z, Tetrahedron_1st_order* tetr); __attribute__((optimize("O1")));

	bool point_in_tetr(int base_node_index, float dx, float dy, float dz, Tetrahedron* tetr);
	bool point_in_tetr(int base_node_index, float dx, float dy, float dz, Tetrahedron_1st_order* tetr);
	Tetrahedron_1st_order* find_owner_tetr(int base_node, float dx, float dy, float dz, ElasticNode* node);
	int interpolate(ElasticNode* node, Tetrahedron* tetr);
	int log_quality_stats();
	int do_next_step();
	float get_max_possible_tau();
	float get_min_h();
	float get_max_h();
	Tetrahedron* get_tetrahedron(int index);
	ElasticNode* get_node(int index);

	void add_node(ElasticNode* node);
	void add_tetr(Tetrahedron_1st_order* tetr);

	//vector<ElasticNode> nodes;
	vector<Tetrahedron_1st_order> tetrs;

	TetrMesh_1stOrder_stats mesh_stats;

private:
	void clear_mesh_stats();

	float calc_determ_pure_tetr(int node1, int node2, int node3, int ref_node);
	float calc_determ_with_shift(int node1, int node2, int node3, int base_node, float dx, float dy, float dz);

	float tetr_h(int i);
	int do_next_part_step(float tau, int stage);
	void move_coords(float tau);
	int set_stress(float tau);
	int proceed_rheology();

	void shift_coordinates(Tetrahedron* tetr, float ext_x, float ext_y, float ext_z);

	vector<ElasticNode> new_nodes;

	quick_math qm_engine;
};

#include "TetrMesh_1stOrder.inl"

#endif
