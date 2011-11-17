#ifndef _GCM_TETR_MESH_1ST_ORDER_H
#define _GCM_TETR_MESH_1ST_ORDER_H  1

#include <sstream>
#include <algorithm>

#include "TetrMesh.h"
#include "../system/quick_math.h"
#include "../datatypes/ElasticNode.h"
#include "../datatypes/Tetrahedron_1st_order.h"
#include "../datatypes/Triangle.h"
#include "../methods/GCM_Tetr_Elastic_Interpolation_1stOrder.h"
#include "../rheotypes/VoidRheologyCalculator.h"

using std::stringstream;
using std::sort;
using std::unique;
using std::ios;

//#define DEBUG_MESH_GEOMETRY 1

class TetrMesh_1stOrder : public TetrMesh
{
public:
	TetrMesh_1stOrder();
	~TetrMesh_1stOrder();

	int load_msh_file(char* file_name);
        int load_node_ele_files(char* node_file_name, char* ele_file_name);
	int load_gmv_file(char* file_name);

	bool point_in_tetr(int base_node_index, float dx, float dy, float dz, Tetrahedron* tetr);
	bool point_in_tetr(int base_node_index, float dx, float dy, float dz, Tetrahedron_1st_order* tetr);
	Tetrahedron_1st_order* find_owner_tetr(ElasticNode* node, float dx, float dy, float dz);
	int interpolate(ElasticNode* node, Tetrahedron* tetr);
	int log_mesh_quality_stats();
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

	vector<Triangle> border;

	int find_border_node_normal(int border_node_index, float* x, float* y, float* z);

	int set_stress(float tau);

private:
	int pre_process_mesh();

	int find_border_elem_normal(int border_element_index, float* x, float* y, float* z);

	float calc_determ_pure_tetr(int node1, int node2, int node3, int ref_node);
	float calc_determ_with_shift(int node1, int node2, int node3, int base_node, float dx, float dy, float dz);

	float tetr_h(int i);
	int do_next_part_step(float tau, int stage);
	void move_coords(float tau);
	int proceed_rheology();

	vector<ElasticNode> new_nodes;

	quick_math qm_engine;
};

#include "TetrMesh_1stOrder.inl"

#endif
