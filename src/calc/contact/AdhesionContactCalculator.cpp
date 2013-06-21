#include "AdhesionContactCalculator.h"

AdhesionContactCalculator::AdhesionContactCalculator()
{
	U_gsl = gsl_matrix_alloc (18, 18);
	om_gsl = gsl_vector_alloc (18);
	x_gsl = gsl_vector_alloc (18);
	p_gsl = gsl_permutation_alloc (18);
};

AdhesionContactCalculator::~AdhesionContactCalculator()
{
	gsl_matrix_free(U_gsl);
	gsl_vector_free(om_gsl);
	gsl_vector_free(x_gsl);
	gsl_permutation_free(p_gsl);
};

void AdhesionContactCalculator::do_calc(ElasticNode* new_node, ElasticMatrix3D* matrix, float* values[], bool inner[], ElasticMatrix3D* virt_matrix, float* virt_values[], bool virt_inner[], float outer_normal[], float scale)
{
	// Here we will store (omega = Matrix_OMEGA * u)
	float omega[9];
	float virt_omega[9];

	int posInEq18 = 0;
	int curNN = 0;

	// For all omegas of real node
	for(int i = 0; i < 9; i++)
	{
		// If omega is 'inner'
		if(inner[i])
		{
			// omega on new time layer is equal to omega on previous time layer along characteristic
			omega[i] = 0;
			for( int j = 0; j < 9; j++ ) {
				omega[i] += matrix->U(i,j) * values[i][j];
			}

			// then we must set the corresponding values of the 18x18 matrix
			gsl_vector_set( om_gsl, 6 * curNN + posInEq18, omega[i] );

			for( int j = 0; j < 9; j++ ) {
				gsl_matrix_set( U_gsl, 6 * curNN + posInEq18, j, matrix->U( i, j ) );
			}
			for( int j = 9; j < 18; j++ ) {
				gsl_matrix_set( U_gsl, 6 * curNN + posInEq18, j, 0 );
			}
			posInEq18++;
		}
	}

	posInEq18 = 0;
	curNN = 1;
	// For all omegas of virtual node
	for(int i = 0; i < 9; i++)
	{
		// If omega is 'inner'
		if(virt_inner[i])
		{
			// omega on new time layer is equal to omega on previous time layer along characteristic
			virt_omega[i] = 0;
			for( int j = 0; j < 9; j++ ) {
				virt_omega[i] += virt_matrix->U(i,j) * virt_values[i][j];
			}

			// then we must set the corresponding values of the 18x18 matrix
			gsl_vector_set( om_gsl, 6 * curNN + posInEq18, virt_omega[i] );

			for( int j = 0; j < 9; j++ ) {
				gsl_matrix_set( U_gsl, 6 * curNN + posInEq18, j, 0 );
			}
			for( int j = 9; j < 18; j++ ) {
				gsl_matrix_set( U_gsl, 6 * curNN + posInEq18, j, virt_matrix->U( i, j - 9 ) );
			}
			posInEq18++;
		}
	}

	// Clear the rest 6 rows of the matrix
	for( int strN = 12; strN < 18; strN++ ) {
		for( int colN = 0; colN < 18; colN++ ) {
			gsl_matrix_set( U_gsl, strN, colN, 0 );
		}
	}

	for( int strN = 12; strN < 18; strN++ ) {
		gsl_vector_set( om_gsl, strN, 0 );
	}

	// Equality of velocities
	gsl_matrix_set( U_gsl, 12, 0, 1 );
	gsl_matrix_set( U_gsl, 12, 9, -1 );
	gsl_matrix_set( U_gsl, 13, 1, 1 );
	gsl_matrix_set( U_gsl, 13, 10, -1 );
	gsl_matrix_set( U_gsl, 14, 2, 1 );
	gsl_matrix_set( U_gsl, 14, 11, -1 );		
	
	// Equality of normal and tangential stress
	// We use outer normal to find total stress vector (sigma * n) - sum of normal and shear - and tell it is equal
	// TODO - is it ok?
	// TODO - never-ending questions - is everything ok with (x-y-z) and (ksi-eta-dzeta) basises?

	// TODO FIXME - it works now because exactly the first axis is the only one where contact is possible
	// and it coincides with outer normal
	gsl_matrix_set(U_gsl, 15, 3, outer_normal[0]);
	gsl_matrix_set(U_gsl, 15, 4, outer_normal[1]);
	gsl_matrix_set(U_gsl, 15, 5, outer_normal[2]);

	gsl_matrix_set(U_gsl, 15, 12, -outer_normal[0]);
	gsl_matrix_set(U_gsl, 15, 13, -outer_normal[1]);
	gsl_matrix_set(U_gsl, 15, 14, -outer_normal[2]);


	gsl_matrix_set(U_gsl, 16, 4, outer_normal[0]);
	gsl_matrix_set(U_gsl, 16, 6, outer_normal[1]);
	gsl_matrix_set(U_gsl, 16, 7, outer_normal[2]);

	gsl_matrix_set(U_gsl, 16, 13, -outer_normal[0]);
	gsl_matrix_set(U_gsl, 16, 15, -outer_normal[1]);
	gsl_matrix_set(U_gsl, 16, 16, -outer_normal[2]);


	gsl_matrix_set(U_gsl, 17, 5, outer_normal[0]);
	gsl_matrix_set(U_gsl, 17, 7, outer_normal[1]);
	gsl_matrix_set(U_gsl, 17, 8, outer_normal[2]);

	gsl_matrix_set(U_gsl, 17, 14, -outer_normal[0]);
	gsl_matrix_set(U_gsl, 17, 16, -outer_normal[1]);
	gsl_matrix_set(U_gsl, 17, 17, -outer_normal[2]);


	// Tmp value for GSL solver
	int s;
	gsl_linalg_LU_decomp (U_gsl, p_gsl, &s);
	gsl_linalg_LU_solve (U_gsl, p_gsl, om_gsl, x_gsl);

	// Just get first 9 values (real node) and dump the rest 9 (virt node)
	for(int j = 0; j < 9; j++)
		new_node->values[j] = gsl_vector_get(x_gsl, j);

};
