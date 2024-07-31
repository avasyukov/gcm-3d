#ifndef _GCM_NAIVE_CONTACT_SLIDING_CALCULATOR_H
#define _GCM_NAIVE_CONTACT_SLIDING_CALCULATOR_H  1

#include <gsl/gsl_linalg.h>

#include "libgcm/calc/contact/ContactCalculator.hpp"
#include "libgcm/calc/border/FreeNaiveBorderCalculator.hpp"
#include "libgcm/Math.hpp"
#include "libgcm/Logging.hpp"

namespace gcm
{
    class SlidingNaiveContactCalculator : public ContactCalculator
    {
    public:
        SlidingNaiveContactCalculator();
        ~SlidingNaiveContactCalculator();
        void doCalc(CalcNode& cur_node, CalcNode& new_node, CalcNode& virt_node,
                                RheologyMatrixPtr matrix, std::vector<CalcNode>& previousNodes, bool inner[],
                                RheologyMatrixPtr virt_matrix, std::vector<CalcNode>& virtPreviousNodes, bool virt_inner[],
                                float outer_normal[], float scale);
    private:
        USE_LOGGER;
        FreeNaiveBorderCalculator *fbc;
		bool isFreeBorder(CalcNode& node, CalcNode& virt_node, float outer_normal[]);
    };
}

#endif
