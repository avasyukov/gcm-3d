#ifndef _GCM_CONTACT_SLIDING_CALCULATOR_H
#define _GCM_CONTACT_SLIDING_CALCULATOR_H  1

#include <gsl/gsl_linalg.h>

#include "libgcm/calc/contact/ContactCalculator.hpp"
#include "libgcm/calc/border/FreeBorderCalculator.hpp"
#include "libgcm/Math.hpp"


class SlidingContactCalculator : public ContactCalculator
{
public:
    SlidingContactCalculator();
    ~SlidingContactCalculator();
    void doCalc(CalcNode& cur_node, CalcNode& new_node, CalcNode& virt_node,
                            RheologyMatrix3D& matrix, vector<CalcNode>& previousNodes, bool inner[],
                            RheologyMatrix3D& virt_matrix, vector<CalcNode>& virtPreviousNodes, bool virt_inner[],
                            float outer_normal[], float scale);
    inline string getType() {
        return "SlidingContactCalculator";
    }

protected:

private:
    FreeBorderCalculator *fbc;
    // Used for border calculation
    gsl_matrix *U_gsl;
    gsl_vector *om_gsl;
    gsl_vector *x_gsl;
    gsl_permutation *p_gsl;
};

#endif
