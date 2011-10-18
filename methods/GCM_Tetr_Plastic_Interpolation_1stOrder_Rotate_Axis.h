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

//#define DEBUG_AXIS_RAND 1

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
	ElasticMatrix3D* elastic_matrix3d[3];
	gsl_matrix *U_gsl;
	gsl_vector *om_gsl;
	gsl_vector *x_gsl;
	gsl_permutation *p_gsl;
	int prepare_part_step(ElasticNode* cur_node, int stage);
	void drop_deviator(ElasticNode* cur_node, ElasticNode* new_node);
	int create_random_axis(ElasticNode* cur_node, TetrMesh* mesh);

	// These functions are used by create_random_axis() internally
	void create_rotation_matrix(int node_num, float phi, float psi, float teta);
	void create_rotation_matrix(int node_num, float x, float y, float z, float teta);
	void find_inverse_matrix(int node_num);

	basis* random_axis;	// New random basises for different nodes
	basis* random_axis_inv;
	int basis_quantity;

	quick_math qm_engine;
};

#include "GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis.inl"

#endif
