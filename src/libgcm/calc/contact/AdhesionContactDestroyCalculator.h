#ifndef _GCM_CONTACT_ADHESION_D_CALCULATOR_H
#define _GCM_CONTACT_ADHESION_D_CALCULATOR_H  1

#include <gsl/gsl_linalg.h>

#include "libgcm/calc/contact/ContactCalculator.h"
#include "libgcm/calc/contact/AdhesionContactCalculator.h"
#include "libgcm/calc/contact/SlidingContactCalculator.h"


class AdhesionContactDestroyCalculator : public ContactCalculator
{
public:
    AdhesionContactDestroyCalculator();
    ~AdhesionContactDestroyCalculator();
    void doCalc(CalcNode& cur_node, CalcNode& new_node, CalcNode& virt_node,
                            RheologyMatrix3D& matrix, vector<CalcNode>& previousNodes, bool inner[],
                            RheologyMatrix3D& virt_matrix, vector<CalcNode>& virtPreviousNodes, bool virt_inner[],
                            float outer_normal[], float scale);
    inline string getType() {
        return "AdhesionContactDestroyCalculator";
    }

protected:

private:
    SlidingContactCalculator* scc;
    AdhesionContactCalculator* acc;
};

#endif
