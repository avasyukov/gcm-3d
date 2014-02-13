#include "BorderCondition.h"

#include "util/forms/PulseForm.h"
#include "node/CalcNode.h"

gcm::BorderCondition::BorderCondition() {
	area = NULL;
	form = NULL;
	calc = NULL;
}

gcm::BorderCondition::BorderCondition(Area* _area, PulseForm* _form, BorderCalculator* _calc) {
	area = _area;
	form = _form;
	calc = _calc;
}

gcm::BorderCondition::~BorderCondition() {
	
}

void gcm::BorderCondition::do_calc(float time, CalcNode* cur_node, CalcNode* new_node, ElasticMatrix3D* matrix, float* values[], bool inner[], float outer_normal[])
{
	memcpy(new_node->coords, cur_node->coords, 3*sizeof(float) );
	calc->do_calc(cur_node, new_node, matrix, values, inner, outer_normal, form->calcMagnitudeNorm(time, cur_node->coords, area) );
};
