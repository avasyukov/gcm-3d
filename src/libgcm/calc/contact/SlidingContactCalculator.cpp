#include "calc/contact/SlidingContactCalculator.h"

#include "calc/border/FreeBorderCalculator.h"
#include "node/CalcNode.h"

SlidingContactCalculator::SlidingContactCalculator()
{
	U_gsl = gsl_matrix_alloc (18, 18);
	om_gsl = gsl_vector_alloc (18);
	x_gsl = gsl_vector_alloc (18);
	p_gsl = gsl_permutation_alloc (18);
};

SlidingContactCalculator::~SlidingContactCalculator()
{
	gsl_matrix_free(U_gsl);
	gsl_vector_free(om_gsl);
	gsl_vector_free(x_gsl);
	gsl_permutation_free(p_gsl);
};

void SlidingContactCalculator::do_calc(CalcNode* cur_node, CalcNode* new_node, CalcNode* virt_node, ElasticMatrix3D* matrix, float* values[], bool inner[], ElasticMatrix3D* virt_matrix, float* virt_values[], bool virt_inner[], float outer_normal[], float scale)
{
	float local_n[3][3];
	local_n[0][0] = outer_normal[0];
	local_n[0][1] = outer_normal[1];
	local_n[0][2] = outer_normal[2];

	createLocalBasis(local_n[0], local_n[1], local_n[2]);

	//---------------------------------------Check if nodes fall apart
	float vel_rel[3] = {cur_node->values[0]+virt_node->values[0],
		   	    cur_node->values[1]+virt_node->values[1],
			    cur_node->values[2]+virt_node->values[2]};
	float force_cur[3] = {cur_node->values[3]*outer_normal[0]+cur_node->values[4]*outer_normal[1]+cur_node->values[5]*outer_normal[2],
			  cur_node->values[4]*outer_normal[0]+cur_node->values[6]*outer_normal[1]+cur_node->values[7]*outer_normal[2],
			  cur_node->values[5]*outer_normal[0]+cur_node->values[7]*outer_normal[1]+cur_node->values[8]*outer_normal[2]};
	
	float vel_abs =  -scalarProduct(vel_rel,outer_normal);
	float force_cur_abs = scalarProduct(force_cur,outer_normal);
	
	float eps = 0.0005;
	bool free_border = false;

	if (vel_abs < -eps) 			//first check relative speed
		free_border = true;
	else if (vel_abs < eps)			//if relative speed is small, we check force
		if (force_cur_abs > -eps)
			free_border = true;

	if (free_border) 		
	{
		FreeBorderCalculator *fbc = new FreeBorderCalculator();
		fbc->do_calc(cur_node, new_node, matrix, values, inner, outer_normal, scale);
		delete fbc;
		return;
	}
	//--------------------------------------------------------------------


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

	// Normal velocities are equal
	gsl_matrix_set( U_gsl, 12, 0, local_n[0][0]);
	gsl_matrix_set( U_gsl, 12, 1, local_n[0][1]);
	gsl_matrix_set( U_gsl, 12, 2, local_n[0][2]);
	gsl_matrix_set( U_gsl, 12, 9,  - local_n[0][0]);
	gsl_matrix_set( U_gsl, 12, 10, - local_n[0][1]);
	gsl_matrix_set( U_gsl, 12, 11, - local_n[0][2]);
	
	// We use outer normal to find total stress vector (sigma * n) - sum of normal and shear - and tell it is equal
	// TODO - is it ok?
	// TODO - never-ending questions - is everything ok with (x-y-z) and (ksi-eta-dzeta) basises?

	// TODO FIXME - it works now because exactly the first axis is the only one where contact is possible
	// and it coincides with outer normal

	// Normal stresses are equal
	gsl_matrix_set(U_gsl, 13, 3, local_n[0][0] * local_n[0][0]);
	gsl_matrix_set(U_gsl, 13, 4, 2 * local_n[0][1] * local_n[0][0]);
	gsl_matrix_set(U_gsl, 13, 5, 2 * local_n[0][2] * local_n[0][0]);
	gsl_matrix_set(U_gsl, 13, 6, local_n[0][1] * local_n[0][1]);
	gsl_matrix_set(U_gsl, 13, 7, 2 * local_n[0][2] * local_n[0][1]);
	gsl_matrix_set(U_gsl, 13, 8, local_n[0][2] * local_n[0][2]);

	gsl_matrix_set(U_gsl, 13, 12, - local_n[0][0] * local_n[0][0]);
	gsl_matrix_set(U_gsl, 13, 13, - 2 * local_n[0][1] * local_n[0][0]);
	gsl_matrix_set(U_gsl, 13, 14, - 2 * local_n[0][2] * local_n[0][0]);
	gsl_matrix_set(U_gsl, 13, 15, - local_n[0][1] * local_n[0][1]);
	gsl_matrix_set(U_gsl, 13, 16, - 2 * local_n[0][2] * local_n[0][1]);
	gsl_matrix_set(U_gsl, 13, 17, - local_n[0][2] * local_n[0][2]);

	// Tangential stresses are zero

	gsl_matrix_set(U_gsl, 14, 3, - (local_n[0][0] * local_n[1][0]) );
	gsl_matrix_set(U_gsl, 14, 4, - (local_n[0][1] * local_n[1][0] + local_n[0][0] * local_n[1][1]) );
	gsl_matrix_set(U_gsl, 14, 5, - (local_n[0][2] * local_n[1][0] + local_n[0][0] * local_n[1][2]) );
	gsl_matrix_set(U_gsl, 14, 6, - (local_n[0][1] * local_n[1][1]) );
	gsl_matrix_set(U_gsl, 14, 7, - (local_n[0][2] * local_n[1][1] + local_n[0][1] * local_n[1][2]) );
	gsl_matrix_set(U_gsl, 14, 8, - (local_n[0][2] * local_n[1][2]) );

	gsl_matrix_set(U_gsl, 15, 3, - (local_n[0][0] * local_n[2][0]) );
	gsl_matrix_set(U_gsl, 15, 4, - (local_n[0][1] * local_n[2][0] + local_n[0][0] * local_n[2][1]) );
	gsl_matrix_set(U_gsl, 15, 5, - (local_n[0][2] * local_n[2][0] + local_n[0][0] * local_n[2][2]) );
	gsl_matrix_set(U_gsl, 15, 6, - (local_n[0][1] * local_n[2][1]) );
	gsl_matrix_set(U_gsl, 15, 7, - (local_n[0][2] * local_n[2][1] + local_n[0][1] * local_n[2][2]) );
	gsl_matrix_set(U_gsl, 15, 8, - (local_n[0][2] * local_n[2][2]) );


	gsl_matrix_set(U_gsl, 16, 12, - (local_n[0][0] * local_n[1][0]) );
	gsl_matrix_set(U_gsl, 16, 13, - (local_n[0][1] * local_n[1][0] + local_n[0][0] * local_n[1][1]) );
	gsl_matrix_set(U_gsl, 16, 14, - (local_n[0][2] * local_n[1][0] + local_n[0][0] * local_n[1][2]) );
	gsl_matrix_set(U_gsl, 16, 15, - (local_n[0][1] * local_n[1][1]) );
	gsl_matrix_set(U_gsl, 16, 16, - (local_n[0][2] * local_n[1][1] + local_n[0][1] * local_n[1][2]) );
	gsl_matrix_set(U_gsl, 16, 17, - (local_n[0][2] * local_n[1][2]) );

	gsl_matrix_set(U_gsl, 17, 12, - (local_n[0][0] * local_n[2][0]) );
	gsl_matrix_set(U_gsl, 17, 13, - (local_n[0][1] * local_n[2][0] + local_n[0][0] * local_n[2][1]) );
	gsl_matrix_set(U_gsl, 17, 14, - (local_n[0][2] * local_n[2][0] + local_n[0][0] * local_n[2][2]) );
	gsl_matrix_set(U_gsl, 17, 15, - (local_n[0][1] * local_n[2][1]) );
	gsl_matrix_set(U_gsl, 17, 16, - (local_n[0][2] * local_n[2][1] + local_n[0][1] * local_n[2][2]) );
	gsl_matrix_set(U_gsl, 17, 17, - (local_n[0][2] * local_n[2][2]) );


	// Tmp value for GSL solver
	int s;
	gsl_linalg_LU_decomp (U_gsl, p_gsl, &s);
	gsl_linalg_LU_solve (U_gsl, p_gsl, om_gsl, x_gsl);

	// Just get first 9 values (real node) and dump the rest 9 (virt node)
	for(int j = 0; j < 9; j++)
		new_node->values[j] = gsl_vector_get(x_gsl, j);

};
