#ifndef _GCM_CONTACT_CONDITION_H
#define _GCM_CONTACT_CONDITION_H 1

#include "util/areas/Area.h"
#include "util/forms/PulseForm.h"
#include "calc/contact/ContactCalculator.h"

class ContactCondition
{
public:
	Area* area;
	PulseForm* form;
	ContactCalculator* calc;
	void do_calc(float time, float* cur_coords, CalcNode* new_node, ElasticMatrix3D* matrix, float* values[], bool inner[], ElasticMatrix3D* virt_matrix, float* virt_values[], bool virt_inner[], float outer_normal[]);
};

#endif
