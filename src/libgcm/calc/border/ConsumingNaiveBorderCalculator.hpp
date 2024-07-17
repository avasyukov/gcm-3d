#ifndef _GCM_CONS_NBORDER_CALCULATOR_H
#define _GCM_CONS_NBORDER_CALCULATOR_H  1

#include <gsl/gsl_linalg.h>

#include "libgcm/calc/border/BorderCalculator.hpp"

namespace gcm
{
    class ConsumingNaiveBorderCalculator : public BorderCalculator
    {
    public:
        ConsumingNaiveBorderCalculator();
        ~ConsumingNaiveBorderCalculator();
        void doCalc(CalcNode& cur_node, CalcNode& new_node, RheologyMatrixPtr matrix,
                                std::vector<CalcNode>& previousNodes, bool inner[],
                                float outer_normal[], float scale);

        inline std::string getType() {
            return "ConsumingNaiveBorderCalculator";
        }

        void setParameters(const xml::Node& params);

    protected:

    private:
        USE_LOGGER;
    };
}
#endif
