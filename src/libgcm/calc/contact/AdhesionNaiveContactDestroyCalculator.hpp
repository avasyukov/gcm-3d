#ifndef _GCM_NAIVE_CONTACT_ADHESION_D_CALCULATOR_H
#define _GCM_NAIVE_CONTACT_ADHESION_D_CALCULATOR_H  1

#include <gsl/gsl_linalg.h>

#include "libgcm/calc/contact/ContactCalculator.hpp"
#include "libgcm/calc/contact/AdhesionNaiveContactCalculator.hpp"
#include "libgcm/calc/contact/SlidingNaiveContactCalculator.hpp"


namespace gcm
{
    class AdhesionNaiveContactDestroyCalculator : public ContactCalculator
    {
    public:
        AdhesionNaiveContactDestroyCalculator();
        ~AdhesionNaiveContactDestroyCalculator();
        void doCalc(CalcNode& cur_node, CalcNode& new_node, CalcNode& virt_node,
                                RheologyMatrixPtr matrix, std::vector<CalcNode>& previousNodes, bool inner[],
                                RheologyMatrixPtr virt_matrix, std::vector<CalcNode>& virtPreviousNodes, bool virt_inner[],
                                float outer_normal[], float scale);
    private:
        SlidingNaiveContactCalculator* scc;
        AdhesionNaiveContactCalculator* acc;
    };
}
#endif
