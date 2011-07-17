#ifndef _GCM_TETR_PLASTIC_INTERPOLATION_1ST_ORDER_ADVANCED_H
#define _GCM_TETR_PLASTIC_INTERPOLATION_1ST_ORDER_ADVANCED_H  1

#include "TetrNumericalMethod.h"
#include "../datatypes/ElasticMatrix3D.h"
#include <gsl/gsl_linalg.h>

#include <sstream>
using std::stringstream;

using std::string;
using std::vector;

class GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced : public TetrNumericalMethod
	// TODO may be we should inherit methods from GCM_Tetr_Plastic_Interpolation_1stOrder
{
public:
	GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced();
	~GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced();
	int do_next_part_step(ElasticNode* cur_node, ElasticNode* new_node, float time_step, int stage, TetrMesh* mesh);
	int get_number_of_stages();
	float get_max_lambda(ElasticNode* node);
protected:
	ElasticMatrix3D* elastic_matrix3d[3];
	gsl_matrix *U_gsl;
	gsl_vector *om_gsl;
	gsl_vector *x_gsl;
	gsl_permutation *p_gsl;
	int prepare_part_step(ElasticNode* cur_node, int stage);
	void drop_deviator(ElasticNode* cur_node, ElasticNode* new_node);
};

#include "GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced.inl"

#endif
