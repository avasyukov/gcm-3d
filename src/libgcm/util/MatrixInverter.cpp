#include "MatrixInverter.hpp"

MatrixInverter::MatrixInverter() {
	U = gsl_matrix_alloc (GCM_MATRIX_SIZE, GCM_MATRIX_SIZE);
	gsl_matrix_set_zero(U);
}

MatrixInverter::~MatrixInverter() {
	gsl_matrix_free(U);
}

void gcm::MatrixInverter::inv(gcm_matrix &result)
{
	// Invert the matrix using gsl library
	gsl_set_error_handler_off();
	
	gsl_matrix* U1 = gsl_matrix_alloc (GCM_MATRIX_SIZE, GCM_MATRIX_SIZE);
	gsl_permutation* perm = gsl_permutation_alloc (GCM_MATRIX_SIZE);  
	int k;
	
	int status = gsl_linalg_LU_decomp (U, perm, &k);
	if (status) {
		LOG_DEBUG("gsl_linalg_LU_decomp failed");
		THROW_INVALID_ARG("gsl_linalg_LU_decomp failed");
	}
	status = gsl_linalg_LU_invert (U, perm, U1);
	if (status) {
		LOG_DEBUG("gsl_linalg_LU_invert failed");
		THROW_INVALID_ARG("gsl_linalg_LU_invert failed");
	}
	
	for(int i = 0; i < GCM_MATRIX_SIZE; i++)
		for(int j = 0; j < GCM_MATRIX_SIZE; j++)
			result(i,j) = gsl_matrix_get(U1, i,j);
};

void gcm::MatrixInverter::setColumn(double *Clmn, int num)
{
	for (int i = 0; i < GCM_MATRIX_SIZE; i++) {
		gsl_matrix_set(U, i, num, Clmn[i]);
	}
}

void gcm::MatrixInverter::setUnity(int i1, int j1, int i2, int j2, int i3, int j3) 
{
	gsl_matrix_set(U, i1, j1, 1);
	gsl_matrix_set(U, i2, j2, 1);
	gsl_matrix_set(U, i3, j3, 1);
}
