#ifndef _GCM_FREE_NBORDER_CALCULATOR_H
#define _GCM_FREE_NBORDER_CALCULATOR_H  1

#include <gsl/gsl_linalg.h>

#include "libgcm/calc/border/BorderCalculator.hpp"

namespace gcm
{
    class FreeNaiveBorderCalculator : public BorderCalculator
    {
    public:
        FreeNaiveBorderCalculator();
        ~FreeNaiveBorderCalculator();
        void doCalc(CalcNode& cur_node, CalcNode& new_node, RheologyMatrixPtr matrix,
                                std::vector<CalcNode>& previousNodes, bool inner[],
                                float outer_normal[], float scale);

        inline std::string getType() {
            return "FreeNaiveBorderCalculator";
        }

        void setParameters(const xml::Node& params);

    protected:

    private:
        USE_LOGGER;
    };
}
#endif
