#ifndef _GCM_EXTERNAL_VALUES_CALCULATOR_H
#define _GCM_EXTERNAL_VALUES_CALCULATOR_H  1

#include "BorderCalculator.h"
#include <gsl/gsl_linalg.h>

class ExternalValuesCalculator : public BorderCalculator
{
public:
	ExternalValuesCalculator();
	~ExternalValuesCalculator();
	void do_calc(CalcNode* new_node, ElasticMatrix3D* matrix, float* values[], bool inner[], float outer_normal[], float scale);
	inline string getType() {
		return "ExternalValuesCalculator";
	}
	void set_parameters(int vars[], float vals[]);

protected:

private:
	int vars_index[3];
	float vars_values[3];

	// Used for border calculation
	gsl_matrix *U_gsl;
	gsl_vector *om_gsl;
	gsl_vector *x_gsl;
	gsl_permutation *p_gsl;
};

#endif
