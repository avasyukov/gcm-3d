#ifndef _GCM_SMOOTH_BORDER_CALCULATOR_H
#define _GCM_SMOOTH_BORDER_CALCULATOR_H  1

#include "BorderCalculator.h"
#include <gsl/gsl_linalg.h>

class SmoothBorderCalculator : public BorderCalculator
{
public:
	SmoothBorderCalculator();
	~SmoothBorderCalculator();
	void do_calc(CalcNode* cur_node, CalcNode* new_node, ElasticMatrix3D* matrix, float* values[], bool inner[], float outer_normal[], float scale);
	
	inline string getType() {
		return "SmoothBorderCalculator";
	}

protected:

private:
	USE_LOGGER;
};

#endif
