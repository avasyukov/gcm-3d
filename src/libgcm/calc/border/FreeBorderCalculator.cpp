#include "libgcm/calc/border/FreeBorderCalculator.hpp"

#include "libgcm/node/CalcNode.hpp"

#include <omp.h>

using namespace gcm;
using std::vector;

FreeBorderCalculator::FreeBorderCalculator()
{
    #pragma omp parallel
    {
        #pragma omp single
        {
            auto nt = omp_get_num_threads();
            U_gsl = new gsl_matrix *[nt];
            om_gsl = new gsl_vector *[nt];
            x_gsl = new gsl_vector *[nt];
            p_gsl = new gsl_permutation *[nt];
        }

        auto n = omp_get_thread_num();
        U_gsl[n] = gsl_matrix_alloc (9, 9);
        om_gsl[n] = gsl_vector_alloc (9);
        x_gsl[n] = gsl_vector_alloc (9);
        p_gsl[n] = gsl_permutation_alloc (9);
    }
    INIT_LOGGER( "gcm.FreeBorderCalculator" );
};

FreeBorderCalculator::~FreeBorderCalculator()
{
#pragma omp parallel
    {
        auto n = omp_get_thread_num();
        gsl_matrix_free(U_gsl[n]);
        gsl_vector_free(om_gsl[n]);
        gsl_vector_free(x_gsl[n]);
        gsl_permutation_free(p_gsl[n]);
        #pragma omp single
        {
            delete[] U_gsl;
            delete[] om_gsl;
            delete[] x_gsl;
            delete[] p_gsl;
        }
    }
};

void FreeBorderCalculator::setParameters(const xml::Node& params)
{
    
};

void FreeBorderCalculator::doCalc(CalcNode& cur_node, CalcNode& new_node, RheologyMatrixPtr matrix,
                            vector<CalcNode>& previousNodes, bool inner[],
                            float outer_normal[], float scale)
{
    assert_eq(previousNodes.size(), 9);

    int outer_count = 3;

    // Tmp value for GSL solver
    int s;

    // Here we will store (omega = Matrix_OMEGA * u)
    float omega[9];

    auto n = omp_get_thread_num();

    for(int i = 0; i < 9; i++)
    {
        // If omega is 'inner' one
        if(inner[i])
        {
            // Calculate omega value
            omega[i] = 0;
            for(int j = 0; j < 9; j++)
            {
                omega[i] += matrix->getU(i,j) * previousNodes[i].values[j];
            }
            // Load appropriate values into GSL containers
            gsl_vector_set(om_gsl[n], i, omega[i]);
            for(int j = 0; j < 9; j++)
                gsl_matrix_set(U_gsl[n], i, j, matrix->getU(i,j));
        }
        // If omega is 'outer' one
        else
        {
            // omega (as right-hand part of OLE) is zero - it is free border, no external stress
            gsl_vector_set(om_gsl[n], i, 0);
            // corresponding string in matrix is zero ...
            for(int j = 0; j < 9; j++)
                gsl_matrix_set(U_gsl[n], i, j, 0);

            // ... except normal and tangential stress
            // We use outer normal to find total stress vector (sigma * n) - sum of normal and shear - and tell it is zero
            // TODO - never-ending questions - is everything ok with (x-y-z) and (ksi-eta-dzeta) basises?

            if ( outer_count == 3 ) {
                gsl_matrix_set(U_gsl[n], i, 3, outer_normal[0]);
                gsl_matrix_set(U_gsl[n], i, 4, outer_normal[1]);
                gsl_matrix_set(U_gsl[n], i, 5, outer_normal[2]);
                outer_count--;
            } else if ( outer_count == 2 ) {
                gsl_matrix_set(U_gsl[n], i, 4, outer_normal[0]);
                gsl_matrix_set(U_gsl[n], i, 6, outer_normal[1]);
                gsl_matrix_set(U_gsl[n], i, 7, outer_normal[2]);
                outer_count--;
            } else if ( outer_count == 1 ) {
                gsl_matrix_set(U_gsl[n], i, 5, outer_normal[0]);
                gsl_matrix_set(U_gsl[n], i, 7, outer_normal[1]);
                gsl_matrix_set(U_gsl[n], i, 8, outer_normal[2]);
                outer_count--;
            }
        }
    }

    // Solve linear equations using GSL tools
    gsl_linalg_LU_decomp (U_gsl[n], p_gsl[n], &s);
    gsl_linalg_LU_solve (U_gsl[n], p_gsl[n], om_gsl[n], x_gsl[n]);

    for(int j = 0; j < 9; j++)
        new_node.values[j] = gsl_vector_get(x_gsl[n], j);

};
