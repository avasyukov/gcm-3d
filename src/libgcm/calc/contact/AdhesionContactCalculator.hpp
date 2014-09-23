#ifndef _GCM_CONTACT_ADHESION_CALCULATOR_H
#define _GCM_CONTACT_ADHESION_CALCULATOR_H  1

#include <gsl/gsl_linalg.h>

#include "libgcm/calc/contact/ContactCalculator.hpp"

namespace gcm
{
    class AdhesionContactCalculator : public ContactCalculator
    {
    public:
        AdhesionContactCalculator();
        ~AdhesionContactCalculator();
        void doCalc(CalcNode& cur_node, CalcNode& new_node, CalcNode& virt_node,
                                RheologyMatrixPtr matrix, std::vector<CalcNode>& previousNodes, bool inner[],
                                RheologyMatrixPtr virt_matrix, std::vector<CalcNode>& virtPreviousNodes, bool virt_inner[],
                                float outer_normal[], float scale);
        inline std::string getType() {
            return "AdhesionContactCalculator";
        }

    protected:

    private:
        // Used for border calculation
        gsl_matrix *U_gsl;
        gsl_vector *om_gsl;
        gsl_vector *x_gsl;
        gsl_permutation *p_gsl;
    };
}
#endif
