#include "ContactCondition.h"

void ContactCondition::do_calc(float time, float* cur_coords, ElasticNode* new_node, ElasticMatrix3D* matrix, float* values[], bool inner[], ElasticMatrix3D* virt_matrix, float* virt_values[], bool virt_inner[], float outer_normal[])
{
	calc->do_calc(new_node, matrix, values, inner, virt_matrix, virt_values, virt_inner, outer_normal, form->calcMagnitudeNorm(time, cur_coords, area) );
};
