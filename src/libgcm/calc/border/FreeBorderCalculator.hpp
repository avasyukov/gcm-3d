#ifndef _GCM_FREE_BORDER_CALCULATOR_H
#define _GCM_FREE_BORDER_CALCULATOR_H  1

#include <gsl/gsl_linalg.h>

#include "libgcm/calc/border/BorderCalculator.hpp"

namespace gcm
{
    class FreeBorderCalculator : public BorderCalculator
    {
    public:
        FreeBorderCalculator();
        ~FreeBorderCalculator();
        void doCalc(CalcNode& cur_node, CalcNode& new_node, RheologyMatrixPtr matrix,
                                std::vector<CalcNode>& previousNodes, bool inner[],
                                float outer_normal[], float scale);

        inline std::string getType() {
            return "FreeBorderCalculator";
        }

        void setParameters(const xml::Node& params);

    protected:

    private:
        USE_LOGGER;
        // Used for border calculation
        //
        // Matrix for transformation into inner Riemann Invariants
        gsl_matrix *A_gsl;
        // Vector for inner Riemann Invariants
        gsl_vector *r_gsl;
        // Matrix for the final solution
        gsl_matrix *M_gsl;
        // right hand side of the final solution
        gsl_matrix_view M_tl_gsl;
        // top left part of matrix M
        gsl_vector *rhs_gsl;
        // part of rhs that is A^Tr
        gsl_vector_view rhs_t_gsl;
        // result of the final solution
        gsl_vector *x_gsl;
        // permutation for LU solver
        gsl_permutation *p_gsl;
    };
}
#endif
