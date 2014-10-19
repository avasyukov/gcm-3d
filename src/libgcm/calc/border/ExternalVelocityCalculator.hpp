#ifndef _GCM_EXTERNAL_VELOCITY_CALCULATOR_H
#define _GCM_EXTERNAL_VELOCITY_CALCULATOR_H  1

#include <gsl/gsl_linalg.h>

#include "libgcm/calc/border/BorderCalculator.hpp"
#include "libgcm/Math.hpp"

namespace gcm
{
    class ExternalVelocityCalculator : public BorderCalculator
    {
    public:
        ExternalVelocityCalculator();
        ~ExternalVelocityCalculator();
        void doCalc(CalcNode& cur_node, CalcNode& new_node, RheologyMatrixPtr matrix,
                                std::vector<CalcNode>& previousNodes, bool inner[],
                                float outer_normal[], float scale) override;
        inline std::string getType() {
            return "ExternalVelocityCalculator";
        }
        void setParameters(const xml::Node& params);

    protected:

    private:
        float normal_v;
        float tangential_v;
        float tangential_direction[3];

        // Used for border calculation
        gsl_matrix *U_gsl;
        gsl_vector *om_gsl;
        gsl_vector *x_gsl;
        gsl_permutation *p_gsl;
    };
}

#endif
