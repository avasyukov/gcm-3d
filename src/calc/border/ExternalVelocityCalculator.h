#ifndef _GCM_EXTERNAL_VELOCITY_CALCULATOR_H
#define _GCM_EXTERNAL_VELOCITY_CALCULATOR_H  1

#include "BorderCalculator.h"
#include <gsl/gsl_linalg.h>
#include "../../Math.h"

class ExternalVelocityCalculator : public BorderCalculator
{
public:
	ExternalVelocityCalculator();
	~ExternalVelocityCalculator();
	void do_calc(ElasticNode* new_node, ElasticMatrix3D* matrix, float* values[], bool inner[], float outer_normal[], float scale);
	inline string getType() {
		return "ExternalVelocityCalculator";
	}
	void set_parameters(float vn, float vt, float xv, float yv, float zv);

protected:

private:
	float normal_v;
	float tangential_v;
	float tangential_direction[3];

	// Used for border calculation
	gsl_matrix *U_gsl;
	gsl_vector *om_gsl;
	gsl_vector *x_gsl;
	gsl_permutation *p_gsl;
};

#endif
