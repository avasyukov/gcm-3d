#ifndef _GCM_TETR_MESH_1ST_ORDER_H
#define _GCM_TETR_MESH_1ST_ORDER_H  1

#include <algorithm>
#include <gsl/gsl_linalg.h>

using std::sort;
using std::unique;

#include "TetrMesh.h"
#include "../datatypes/Triangle.h"
#include "../datatypes/Tetrahedron_1st_order.h"
#include "../system/quick_math.h"
#include "../system/DataBus.h"

//#define DEBUG_MESH_GEOMETRY 1

class TetrMesh_1stOrder : public TetrMesh
{
public:
	TetrMesh_1stOrder();
	~TetrMesh_1stOrder();

//	void attach_data_bus(DataBus* new_data_bus);

	int load_msh_file(char* file_name);
	int load_node_ele_files(char* node_file_name, char* ele_file_name);
	int load_gmv_file(char* file_name);

	bool point_in_tetr(int base_node_index, float dx, float dy, float dz, Tetrahedron* tetr, bool debug);
	bool point_in_tetr(int base_node_index, float dx, float dy, float dz, Tetrahedron* tetr);
	bool point_in_tetr(int base_node_index, float dx, float dy, float dz, Tetrahedron_1st_order* tetr);
	bool point_in_tetr(float x, float y, float z, Tetrahedron* tetr);
	bool point_in_tetr(float x, float y, float z, Tetrahedron_1st_order* tetr);

	Tetrahedron_1st_order* find_owner_tetr(ElasticNode* node, float dx, float dy, float dz);
	Tetrahedron_1st_order* find_owner_tetr(ElasticNode* node, float dx, float dy, float dz, bool debug);
	int interpolate(ElasticNode* node, Tetrahedron* tetr);
	// checks if vector from p0 in direction v with lenght l intersects triangle p1-p2-p3
	// if yes - point of intersection will be returned in p
	bool vector_intersects_triangle(float *p1, float *p2, float *p3, float *p0, float *v, float l, float *p);
	// interpolates value inside of triangle
	bool interpolate_triangle(float *p1, float *p2, float *p3, float *p, float *v1, float *v2, float *v3, float *v, int n);
	int log_mesh_stats();
//	int do_next_step();
//	int do_next_step(float time_step);
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

	int pre_process_mesh();

	void update_current_time(float time_step);

	int do_next_part_step(float tau, int stage);
	void move_coords(float tau);
	int proceed_rheology();
	void clear_data();

private:
	int check_triangle_to_be_border(int v1, int v2, int v3, int tetr_vert, float step_h);
	bool triangleOrientationOk(int node1, int node2, int node3);

	int find_border_elem_normal(int border_element_index, float* x, float* y, float* z);
	int find_border_elem_normal(int v1, int v2, int v3, float* x, float* y, float* z);
	int find_border_elem_normal(float *p1, float *p2, float *p3, float *x, float *y, float *z);

	float calc_determ_pure_tetr(int node1, int node2, int node3, int ref_node);
	float calc_determ_with_shift(int node1, int node2, int node3, int base_node, float dx, float dy, float dz);

	float get_solid_angle(int node_index, int tetr_index);

	float tetr_h(int i);
	int run_mesh_filter();

	vector<ElasticNode> new_nodes;

	gsl_matrix *T;
	gsl_matrix *S;
	gsl_permutation *P;

	quick_math qm_engine;
};

#endif
