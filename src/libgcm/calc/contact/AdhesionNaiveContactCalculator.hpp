#ifndef _GCM_NCONTACT_ADHESION_CALCULATOR_H
#define _GCM_NCONTACT_ADHESION_CALCULATOR_H  1

#include <gsl/gsl_linalg.h>

#include "libgcm/calc/contact/ContactCalculator.hpp"

namespace gcm
{
    class AdhesionNaiveContactCalculator : public ContactCalculator
    {
    public:
        AdhesionNaiveContactCalculator();
        ~AdhesionNaiveContactCalculator();
        void doCalc(CalcNode& cur_node, CalcNode& new_node, CalcNode& virt_node,
                                RheologyMatrixPtr matrix, std::vector<CalcNode>& previousNodes, bool inner[],
                                RheologyMatrixPtr virt_matrix, std::vector<CalcNode>& virtPreviousNodes, bool virt_inner[],
                                float outer_normal[], float scale);
    private:
    };
}
#endif
