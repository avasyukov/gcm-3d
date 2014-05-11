#ifndef _GCM_CONTACT_CONDITION_H
#define _GCM_CONTACT_CONDITION_H 1

#include "libgcm/util/areas/Area.hpp"
#include "libgcm/util/forms/PulseForm.hpp"
#include "libgcm/calc/contact/ContactCalculator.hpp"

namespace gcm {
    class PulseForm;
    class CalcNode;

    class ContactCondition
    {
    protected:
        /** 
         * Parameter of contact condition. It is interpreted by contact calculator.
         * Depending on contact type it can be adhesion treshold, friction coefficient or smth else.
         */
        gcm_real conditionParam;
        
    public:
        ContactCondition();
        ContactCondition(Area* _area, PulseForm* _form, ContactCalculator* _calc);
        ~ContactCondition();

        Area* area;
        PulseForm* form;
        ContactCalculator* calc;
        void doCalc(float time, CalcNode& cur_node, CalcNode& new_node, CalcNode& virt_node,
                            RheologyMatrixPtr matrix, vector<CalcNode>& previousNodes, bool inner[],
                            RheologyMatrixPtr virt_matrix, vector<CalcNode>& virtPreviousNodes, bool virt_inner[],
                            float outer_normal[]);
        
        void setConditionParam(gcm_real param);
        gcm_real getConditionParam() const;
    };
}

#endif
