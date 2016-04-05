#include "libgcm/calc/border/ConsumingBorderCalculator.hpp"

#include "libgcm/node/CalcNode.hpp"

using namespace gcm;
using std::vector;

ConsumingBorderCalculator::ConsumingBorderCalculator()
{
    U_gsl = gsl_matrix_alloc (9, 9);
    om_gsl = gsl_vector_alloc (9);
    x_gsl = gsl_vector_alloc (9);
    p_gsl = gsl_permutation_alloc (9);
    INIT_LOGGER( "gcm.ConsumingBorderCalculator" );
};

ConsumingBorderCalculator::~ConsumingBorderCalculator()
{
    gsl_matrix_free(U_gsl);
    gsl_vector_free(om_gsl);
    gsl_vector_free(x_gsl);
    gsl_permutation_free(p_gsl);
};

void ConsumingBorderCalculator::setParameters(const xml::Node& params)
{
    
};

void ConsumingBorderCalculator::doCalc(CalcNode& cur_node, CalcNode& new_node, RheologyMatrixPtr matrix,
                            vector<CalcNode>& previousNodes, bool inner[],
                            float outer_normal[], float scale)
{
    assert_eq(previousNodes.size(), 9);

    int outer_count = 3;

    // Tmp value for GSL solver
    int s;

    // Here we will store (omega = Matrix_OMEGA * u)
    float omega[9];

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
            gsl_vector_set(om_gsl, i, omega[i]);
            for(int j = 0; j < 9; j++)
                gsl_matrix_set(U_gsl, i, j, matrix->getU(i,j));
        }
        // If omega is 'outer' one
        else
        {
            // Load appropriate values into GSL containers
            gsl_vector_set(om_gsl, i, 0);
            for(int j = 0; j < 9; j++)
                gsl_matrix_set(U_gsl, i, j, matrix->getU(i,j));
        }
    }

	LOG_TRACE("Fucking FBC: outer_count = " << outer_count << "\nMatrix:\n");
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			LOG_TRACE(gsl_matrix_get(U_gsl, i, j) << "\t");
		}
		LOG_TRACE("\n");
	}
			
			
    // Solve linear equations using GSL tools
    gsl_linalg_LU_decomp (U_gsl, p_gsl, &s);
    gsl_linalg_LU_solve (U_gsl, p_gsl, om_gsl, x_gsl);

    for(int j = 0; j < 9; j++)
        new_node.values[j] = gsl_vector_get(x_gsl, j);

};
