#include "ContactCondition.h"
#include "node/CalcNode.h"

gcm::ContactCondition::ContactCondition() {
	area = NULL;
	form = NULL;
	calc = NULL;
}

gcm::ContactCondition::ContactCondition(Area* _area, PulseForm* _form, ContactCalculator* _calc) {
	area = _area;
	form = _form;
	calc = _calc;
}

gcm::ContactCondition::~ContactCondition() {
	
}

void gcm::ContactCondition::do_calc(float time, CalcNode* cur_node, CalcNode* new_node, CalcNode* virt_node, ElasticMatrix3D* matrix, float* values[], bool inner[], ElasticMatrix3D* virt_matrix, float* virt_values[], bool virt_inner[], float outer_normal[])
{
	calc->do_calc(cur_node, new_node, virt_node, matrix, values, inner, virt_matrix, virt_values, virt_inner, outer_normal, form->calcMagnitudeNorm(time, cur_node->coords, area) );
};
