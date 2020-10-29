#include "libgcm/calc/border/FreeBorderCalculator.hpp"

#include "libgcm/node/CalcNode.hpp"

#ifndef __LAMBDA__
#define __LAMBDA__ 0.1
#endif

using namespace gcm;
using std::vector;

FreeBorderCalculator::FreeBorderCalculator()
{
    // equation will be with 9 primal and 3 dual variables
    M_gsl = gsl_matrix_calloc(9 + 3, 9 + 3);
    M_tl_gsl = gsl_matrix_submatrix(M_gsl, 0, 0, 9, 9);

    rhs_gsl = gsl_vector_calloc(9 + 3);
    rhs_t_gsl = gsl_vector_subvector(rhs_gsl, 0, 9);

    // We have 6 inner Riemann invariants
    A_gsl = gsl_matrix_alloc(6, 9);
    r_gsl = gsl_vector_alloc(6);
    

    x_gsl = gsl_vector_alloc(9 + 3);
    p_gsl = gsl_permutation_alloc(9 + 3);

    

    INIT_LOGGER( "gcm.FreeBorderCalculator" );
};

FreeBorderCalculator::~FreeBorderCalculator()
{
    gsl_matrix_free(A_gsl);
    gsl_matrix_free(M_gsl);
    gsl_vector_free(x_gsl);
    gsl_vector_free(rhs_gsl);
    gsl_vector_free(r_gsl);
    gsl_permutation_free(p_gsl);
};

void FreeBorderCalculator::setParameters(const xml::Node& params)
{
    
};

void FreeBorderCalculator::doCalc(CalcNode& cur_node, CalcNode& new_node, RheologyMatrixPtr matrix,
                            vector<CalcNode>& previousNodes, bool inner[],
                            float outer_normal[], float scale)
{
    assert_eq(previousNodes.size(), 9);

    // Tmp value for GSL solver
    int s;

    int k = 0;

    gsl_matrix_set_zero(M_gsl);
    gsl_vector_set_zero(rhs_gsl);

    for(int i = 0; i < 9; i++)
    {
        if(!inner[i])
            continue;
        
        // evaluate k-th Riemann invariant
        double r = 0;
        for(int j = 0; j < 9; j++)
            r += matrix->getU(i,j) * previousNodes[i].values[j];
        gsl_vector_set(r_gsl, k, r);
        
        // filling the matrix A that corresponds k-th inner Riemann invariant
        // TODO: this can be done at construction step, if we somehow get inner/outer list
        for(int j = 0; j < 9; j++)
            gsl_matrix_set(A_gsl, k, j, matrix->getU(i,j));

        k++;
    }

    assert_eq(k, 6);

    // evaluate 2*A^T*A and write it to top left side of M
    gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1., A_gsl, A_gsl, 0., &M_tl_gsl.matrix);

    // add regularization term
    for(int i = 0; i < 9; i++)
        gsl_matrix_set(M_gsl, i, i, gsl_matrix_get(M_gsl, i, i) + __LAMBDA__);
    
    // evaluate A^T*r and write it on rhs 
    gsl_blas_dgemv(CblasTrans, 1., A_gsl, r_gsl, 0., &rhs_t_gsl.vector);

    // Filling in the B and B^T submatrices of M:
    // looks kinda ugly
    // TODO: this can be done at construction step, if we somehow get inner/outer list

    // B:

    gsl_matrix_set(M_gsl, 9 + 0, 3, outer_normal[0]);
    gsl_matrix_set(M_gsl, 9 + 0, 4, outer_normal[1]);
    gsl_matrix_set(M_gsl, 9 + 0, 5, outer_normal[2]);

    gsl_matrix_set(M_gsl, 9 + 1, 4, outer_normal[0]);
    gsl_matrix_set(M_gsl, 9 + 1, 6, outer_normal[1]);
    gsl_matrix_set(M_gsl, 9 + 1, 7, outer_normal[2]);

    gsl_matrix_set(M_gsl, 9 + 2, 5, outer_normal[0]);
    gsl_matrix_set(M_gsl, 9 + 2, 7, outer_normal[1]);
    gsl_matrix_set(M_gsl, 9 + 2, 8, outer_normal[2]);

    // B^T:

    gsl_matrix_set(M_gsl, 3, 9 + 0, outer_normal[0]);
    gsl_matrix_set(M_gsl, 4, 9 + 0, outer_normal[1]);
    gsl_matrix_set(M_gsl, 5, 9 + 0, outer_normal[2]);
                                  
    gsl_matrix_set(M_gsl, 4, 9 + 1, outer_normal[0]);
    gsl_matrix_set(M_gsl, 6, 9 + 1, outer_normal[1]);
    gsl_matrix_set(M_gsl, 7, 9 + 1, outer_normal[2]);
                                  
    gsl_matrix_set(M_gsl, 5, 9 + 2, outer_normal[0]);
    gsl_matrix_set(M_gsl, 7, 9 + 2, outer_normal[1]);
    gsl_matrix_set(M_gsl, 8, 9 + 2, outer_normal[2]);

    LOG_TRACE("Fucking FBC: outer_count = " << outer_count << "\nM:\n");
    for(int i = 0; i < 12; i++) {
        for(int j = 0; j < 12; j++) {
            LOG_TRACE(gsl_matrix_get(M_gsl, i, j) << "\t");
        }
        LOG_TRACE("\n");
    }
    LOG_TRACE("\nrhs:\n");
    for(int i = 0; i < 12; i++)
        LOG_TRACE(gsl_vector_get(rhs_gsl, i) << "\n");
            
            
    // Solve linear equations using GSL tools
    gsl_linalg_LU_decomp(M_gsl, p_gsl, &s);
    gsl_linalg_LU_solve(M_gsl, p_gsl, rhs_gsl, x_gsl);

    for(int j = 0; j < 9; j++)
        new_node.values[j] = gsl_vector_get(x_gsl, j);

};
