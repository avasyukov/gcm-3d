#ifndef _GCM_EXTERNAL_FORCE_NCALCULATOR_H
#define _GCM_EXTERNAL_FORCE_NCALCULATOR_H  1

#include <gsl/gsl_linalg.h>

#include "libgcm/calc/border/BorderCalculator.hpp"
#include "libgcm/Math.hpp"

namespace gcm
{
    class ExternalForceNaiveCalculator : public BorderCalculator
    {
    public:
        ExternalForceNaiveCalculator();
        ~ExternalForceNaiveCalculator();
        void doCalc(CalcNode& cur_node, CalcNode& new_node, RheologyMatrixPtr matrix,
                                std::vector<CalcNode>& previousNodes, bool inner[],
                                float outer_normal[], float scale);
        inline std::string getType() {
            return "ExternalForceNaiveCalculator";
        }
        void setParameters(const xml::Node& params);
        float tangential_direction[3];
        float normal_stress;
        float tangential_stress;

    protected:

    private:

    };
}
#endif
