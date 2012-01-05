#ifndef _GCM_TETR_PLASTIC_INTERPOLATION_1ST_ORDER_ROTATE_AXIS_H
#define _GCM_TETR_PLASTIC_INTERPOLATION_1ST_ORDER_ROTATE_AXIS_H  1

#include "TetrNumericalMethod.h"
#include "../datatypes/ElasticMatrix3D.h"
#include <gsl/gsl_linalg.h>

#include <sstream>
#include <set>
using std::stringstream;

using std::string;
using std::vector;
using std::set;
using std::ios;

#include "../datatypes/Basis.h"

class GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis : public TetrNumericalMethod
	// TODO may be we should inherit methods from GCM_Tetr_Plastic_Interpolation_1stOrder
{
public:
	GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis();
	~GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis();
	int do_next_part_step(ElasticNode* cur_node, ElasticNode* new_node, float time_step, int stage, TetrMesh* mesh);
	int get_number_of_stages();
	float get_max_lambda(ElasticNode* node, TetrMesh* mesh);
protected:
	// Used for real node
	ElasticMatrix3D* elastic_matrix3d[3];
	// Used for interpolated virtual node in case of contact algorithm
	ElasticMatrix3D* virt_elastic_matrix3d[3];
	// Used for border calculation
	gsl_matrix *U_gsl;
	gsl_vector *om_gsl;
	gsl_vector *x_gsl;
	gsl_permutation *p_gsl;
	// Used for contact calculation
	gsl_matrix *U_gsl_18;
	gsl_vector *om_gsl_18;
	gsl_vector *x_gsl_18;
	gsl_permutation *p_gsl_18;

	int prepare_node(ElasticNode* cur_node, ElasticMatrix3D* matrixes[], float time_step, int stage, TetrMesh* mesh, float dksi[], bool inner[], ElasticNode previous_nodes[], float outer_normal[], int ppoint_num[], int basis_num, float value_limiters[]);
	int prepare_node(ElasticNode* cur_node, ElasticMatrix3D* matrixes[], float time_step, int stage, TetrMesh* mesh, float dksi[], bool inner[], ElasticNode previous_nodes[], float outer_normal[], int ppoint_num[], int basis_num, float value_limiters[], bool debug);

	int prepare_part_step(ElasticNode* cur_node, ElasticMatrix3D* matrix, int stage, int basis_num);
	void drop_deviator(ElasticNode* cur_node, ElasticNode* new_node);
	int create_random_axis(ElasticNode* cur_node, TetrMesh* mesh);

	void log_node_diagnostics(ElasticNode* cur_node, int stage, float outer_normal[], TetrMesh* mesh, int basis_num, ElasticMatrix3D* matrixes[], float time_step, ElasticNode previous_nodes[], int ppoint_num[], bool inner[], float dksi[], float value_limiters[]);

	// FIXME
	void create_E_matrix(int node_num);

	// These functions are used by create_random_axis() internally
	void create_rotation_matrix(int node_num, float phi, float psi, float teta);
	void create_rotation_matrix(int node_num, float x, float y, float z, float teta);
	void find_inverse_matrix(int node_num);

	basis* random_axis;	// New random basises for different nodes
	basis* random_axis_inv;
	int basis_quantity;

	int find_nodes_on_previous_time_layer(ElasticNode* cur_node, int stage, TetrMesh* mesh, float alpha, float dksi[], bool inner[], ElasticNode previous_nodes[], float outer_normal[], int ppoint_num[], int basis_num, float value_limiters[]);
	int find_nodes_on_previous_time_layer(ElasticNode* cur_node, int stage, TetrMesh* mesh, float alpha, float dksi[], bool inner[], ElasticNode previous_nodes[], float outer_normal[], int ppoint_num[], int basis_num, float value_limiters[], bool debug);

	quick_math qm_engine;
};

#include "GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis.inl"

#endif
