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

void gcm::BorderCondition::doCalc(float time, CalcNode& cur_node, CalcNode& new_node, RheologyMatrix3D& matrix,
                            vector<CalcNode>& previousNodes, bool inner[], float outer_normal[])
{
    memcpy(new_node.coords, cur_node.coords, 3*sizeof(float) ); // TODO - why is it here?
    calc->doCalc(cur_node, new_node, matrix, previousNodes, inner, outer_normal,
                    form->calcMagnitudeNorm(time, cur_node.coords, area) );
};
