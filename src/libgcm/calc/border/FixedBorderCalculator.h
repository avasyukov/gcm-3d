#ifndef _GCM_FIXED_BORDER_CALCULATOR_H
#define _GCM_FIXED_BORDER_CALCULATOR_H  1

#include "BorderCalculator.h"
#include <gsl/gsl_linalg.h>

class FixedBorderCalculator : public BorderCalculator
{
public:
	FixedBorderCalculator();
	~FixedBorderCalculator();
	void do_calc(CalcNode* cur_node, CalcNode* new_node, ElasticMatrix3D* matrix, float* values[], bool inner[], float outer_normal[], float scale);
	inline string getType() {
		return "FixedBorderCalculator";
	}

protected:

private:
	// Used for border calculation
	gsl_matrix *U_gsl;
	gsl_vector *om_gsl;
	gsl_vector *x_gsl;
	gsl_permutation *p_gsl;
};

#endif
