#include "libgcm/ContactCondition.hpp"

#include "libgcm/node/CalcNode.hpp"

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

void gcm::ContactCondition::doCalc(float time, CalcNode& cur_node, CalcNode& new_node, CalcNode& virt_node,
                            RheologyMatrixPtr matrix, vector<CalcNode>& previousNodes, bool inner[],
                            RheologyMatrixPtr virt_matrix, vector<CalcNode>& virtPreviousNodes, bool virt_inner[],
                            float outer_normal[])
{
    calc->doCalc(cur_node, new_node, virt_node, matrix, previousNodes, inner,
                    virt_matrix, virtPreviousNodes, virt_inner, outer_normal,
                    form->calcMagnitudeNorm(time, cur_node.coords, area) );
};

void gcm::ContactCondition::setConditionParam(gcm_real param)
{
    conditionParam = param;
};

gcm_real gcm::ContactCondition::getConditionParam() const
{
    return conditionParam;
};
