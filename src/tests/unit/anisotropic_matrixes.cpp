#include <time.h>

#include "util/AnisotropicMatrix3D.h"

#include <gtest/gtest.h>

#define ITERATIONS 1000

/*
TEST(AnisotropicMatrix3D, FuzzyMultiplication) {
	srand(time(NULL));

	gcm::AnisotropicMatrix3D m;
	
	gsl_matrix* a = gsl_matrix_alloc (9, 9);
	gsl_matrix* u1 = gsl_matrix_alloc (9, 9);
	gsl_matrix* l = gsl_matrix_alloc (9, 9);
	gsl_matrix* u = gsl_matrix_alloc (9, 9);
	gsl_matrix* tmp = gsl_matrix_alloc(9, 9);
	double x;
	int i;
	
	for(i = 0; i < ITERATIONS; i++ )
	{
		for(int j = 0; j < 9; j++)
			for(int k = 0; k <= j; k++) {
				x = pow(10, k)*(double)rand() / RAND_MAX;
				gsl_matrix_set(a, j, k, x);
				gsl_matrix_set(a, k, j, x);
			}
		m.DecompositeIt(a, u, l, u1);
		
		gsl_blas_dgemm (CblasNoTrans, CblasNoTrans, 1.0, u1, l, 0, tmp);
		gsl_blas_dgemm (CblasNoTrans, CblasNoTrans, 1.0, tmp, u, 0, u1);
		
		for(int j = 0; j < 9; j++)
			for(int k = 0; k < 9; k++)
				ASSERT_NEAR(gsl_matrix_get(a, j, k), gsl_matrix_get(u, j, k), 1e-6);
	}
	
	gsl_matrix_free (a);
	gsl_matrix_free (u1);
	gsl_matrix_free (l);
	gsl_matrix_free (u);
	gsl_matrix_free (tmp);
}
*/
