#include <time.h>

#include "util/AnisotropicMatrix3D.h"

#include <gtest/gtest.h>

#define ITERATIONS 100

TEST(AnisotropicMatrix3D, FuzzyMultiplication) {
	srand(time(NULL));

	gcm::AnisotropicMatrix3D m;
	gsl_matrix* a = gsl_matrix_alloc (9, 9);
	gsl_matrix_complex* u1 = gsl_matrix_complex_alloc (9, 9);
	gsl_matrix_complex* l = gsl_matrix_complex_alloc (9, 9);
	gsl_matrix_complex* u = gsl_matrix_complex_alloc (9, 9);
	gsl_matrix_complex* tmp = gsl_matrix_complex_alloc(9, 9);
	//gsl_complex z;
	//double x;
	
	for( int i = 0; i < ITERATIONS; i++ )
	{
		for(int j = 0; j < 9; j++)
			for(int k = 0; k < 9; k++)
				gsl_matrix_set(a, j, k, 1.0);//(double)rand() / RAND_MAX);
		m.DecompositeIt(a, u1, l, u);
		
		gsl_blas_zgemm (CblasNoTrans, CblasNoTrans, GSL_COMPLEX_ONE, u, l, GSL_COMPLEX_ZERO, tmp);
		gsl_blas_zgemm (CblasNoTrans, CblasNoTrans, GSL_COMPLEX_ONE, tmp, u1, GSL_COMPLEX_ZERO, u);
		
		for(int j = 0; j < 9; j++)
			for(int k = 0; k < 9; k++) {
				ASSERT_NEAR(gsl_matrix_get(a, j, k), GSL_REAL(gsl_matrix_complex_get(u, j, k)), 1e-6);
				ASSERT_DOUBLE_EQ(0.0, GSL_IMAG(gsl_matrix_complex_get(u, j, k)));
			}
	}
	
	gsl_matrix_free (a);
	gsl_matrix_complex_free (u1);
	gsl_matrix_complex_free (l);
	gsl_matrix_complex_free (u);
	gsl_matrix_complex_free (tmp);
}
