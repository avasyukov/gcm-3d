#ifndef _GCM_BORDER_CONDITION_H
#define _GCM_BORDER_CONDITION_H 1

#include <string.h>

#include "libgcm/util/areas/Area.hpp"
#include "libgcm/calc/border/BorderCalculator.hpp"

namespace gcm {
    class PulseForm;
    class CalcNode;

    class BorderCondition
    {
    public:
        BorderCondition();
        BorderCondition(Area* _area, PulseForm* _form, BorderCalculator* _calc);
        ~BorderCondition();

        Area* area;
        PulseForm* form;
        BorderCalculator* calc;
        void doCalc(float time, CalcNode& cur_node, CalcNode& new_node, RheologyMatrixPtr matrix,
                            vector<CalcNode>& previousNodes, bool inner[], float outer_normal[]);
    };
}

#endif
