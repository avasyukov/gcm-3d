#ifndef _GCM_CONTACT_ADHESION_CALCULATOR_H
#define _GCM_CONTACT_ADHESION_CALCULATOR_H  1

#include "ContactCalculator.h"
#include <gsl/gsl_linalg.h>

class AdhesionContactCalculator : public ContactCalculator
{
public:
	AdhesionContactCalculator();
	~AdhesionContactCalculator();
	void do_calc(ElasticNode* new_node, ElasticMatrix3D* matrix, float* values[], bool inner[], ElasticMatrix3D* virt_matrix, float* virt_values[], bool virt_inner[], float outer_normal[], float scale);
	inline string getType() {
		return "AdhesionContactCalculator";
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
