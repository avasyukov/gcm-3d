#include "util/AnisotropicMatrix3D.h"

void gcm::AnisotropicMatrix3D::clear()
{
	A.clear();
	L.clear();
	U.clear();
	U1.clear();
};

void gcm::AnisotropicMatrix3D::getRheologyParameters(initializer_list<gcm_real> params, AnisotropicNumbers& numbers, gcm_real& rho)
{
	int i = 0;
	
	assert(params.size() == ANISOTROPIC_PARAMS_COUNT+1);
	
	auto p = params.begin();
	while (p != params.end()-1)
		numbers.c[i++] = *p;
		
	rho = *p;
};

void gcm::AnisotropicMatrix3D::gslTogcm(gsl_matrix* a, gcm_matrix& b)
{
	for(int i = 0; i < 9; i++)
		for(int j = 0; j < 9; j++)
			b(i, j) = gsl_matrix_get(a, i, j);
};

void gcm::AnisotropicMatrix3D::gcmTogsl(const gcm_matrix& a, gsl_matrix* b)
{
	for(int i = 0; i < 9; i++)
		for(int j = 0; j < 9; j++)
			gsl_matrix_set(b, i, j, a.p[i][j]);
};

void gcm::AnisotropicMatrix3D::clear(gsl_matrix* a)
{
	for(int i = 0; i < 9; i++)
		for(int j = 0; j < 9; j++)
			gsl_matrix_set (a, i, j, 0);
};

void gcm::AnisotropicMatrix3D::realChecker(gsl_matrix_complex* a, gsl_matrix* u)
{
	int i, j;
	gsl_complex z;
	
	for(i = 0; i < 9; i++)
		for(j = 0; j < 9; j++) {
			z = gsl_matrix_complex_get( a, i, j );
			if( fabs( GSL_IMAG( z ) ) > 1e-8 ) THROW_INVALID_INPUT("Vector is complex!");
			gsl_matrix_set(u, i, j, GSL_REAL(z));
		}

};

void gcm::AnisotropicMatrix3D::realChecker(gsl_vector_complex* a, gsl_matrix* l)
{
	int i;
	gsl_complex z;
	
	for(i = 0; i < 9; i++) {
		z = gsl_vector_complex_get(a, i);
		if( fabs( GSL_IMAG( z ) ) > 1e-8 ) THROW_INVALID_INPUT("Value is complex!");
		gsl_matrix_set(l, i, i, GSL_REAL(z));
	}
};

void gcm::AnisotropicMatrix3D::decompositeIt(gsl_matrix* a, gsl_matrix* u, gsl_matrix* l, gsl_matrix* u1) 
{
	int i;

	gsl_matrix* temp = gsl_matrix_alloc(9, 9);
	gsl_matrix_memcpy(temp, a);
	
	// Finding eigenvalues & eigenvectors of 'a'
	gsl_vector_complex* eval = gsl_vector_complex_alloc (9);
	gsl_matrix_complex* evec = gsl_matrix_complex_alloc (9, 9);
	gsl_eigen_nonsymmv_workspace* w = gsl_eigen_nonsymmv_alloc (9);
	gsl_eigen_nonsymmv (temp, eval, evec, w);
	gsl_eigen_nonsymmv_free (w);

	// eval & evec must be real
	clear(l);	
	realChecker(eval, l);
	realChecker(evec, u);
	
	gsl_permutation* perm = gsl_permutation_alloc (9);
	gsl_matrix_memcpy(temp, u);
	
	// Inverting of G matrix
	gsl_linalg_LU_decomp (temp, perm, &i);
	gsl_linalg_LU_invert (temp, perm, u1);
	
	gsl_matrix_free(temp);
	gsl_vector_complex_free(eval);
	gsl_matrix_complex_free(evec);
			
};

void gcm::AnisotropicMatrix3D::createAx(initializer_list<gcm_real> params)
{
	clear();
	
	gcm_real rho;
	AnisotropicNumbers numbers;
	getRheologyParameters(params, numbers, rho);
	
	gsl_matrix* a = gsl_matrix_alloc (9, 9);
	clear(a);
		
	gsl_matrix_set (a, 0, 3, -1/rho);
	gsl_matrix_set (a, 1, 8, -1/rho);
	gsl_matrix_set (a, 2, 7, -1/rho);
	
	gsl_matrix_set (a, 3, 0, -numbers.c[0]);
	gsl_matrix_set (a, 3, 1, -numbers.c[5]);
	gsl_matrix_set (a, 3, 2, -numbers.c[4]);
	
	gsl_matrix_set (a, 4, 0, -numbers.c[1]);
	gsl_matrix_set (a, 4, 1, -numbers.c[10]);
	gsl_matrix_set (a, 4, 2, -numbers.c[9]);
	
	gsl_matrix_set (a, 5, 0, -numbers.c[2]);
	gsl_matrix_set (a, 5, 1, -numbers.c[14]);
	gsl_matrix_set (a, 5, 2, -numbers.c[13]);
	
	gsl_matrix_set (a, 6, 0, -numbers.c[3]);
	gsl_matrix_set (a, 6, 1, -numbers.c[17]);
	gsl_matrix_set (a, 6, 2, -numbers.c[16]);
	
	gsl_matrix_set (a, 7, 0, -numbers.c[4]);
	gsl_matrix_set (a, 7, 1, -numbers.c[19]);
	gsl_matrix_set (a, 7, 2, -numbers.c[18]);
	
	gsl_matrix_set (a, 8, 0, -numbers.c[5]);
	gsl_matrix_set (a, 8, 1, -numbers.c[20]);
	gsl_matrix_set (a, 8, 2, -numbers.c[19]);
	
	gsl_matrix* u1 = gsl_matrix_alloc (9, 9);
	gsl_matrix* l = gsl_matrix_alloc (9, 9);
	gsl_matrix* u = gsl_matrix_alloc (9, 9);
	
	decompositeIt(a, u1, l, u);
	
	gslTogcm(a, A);
	gslTogcm(u, U);
	gslTogcm(l, L);
	gslTogcm(u1, U1);
	
	gsl_matrix_free(a);	
	gsl_matrix_free(u);	
	gsl_matrix_free(l);	
	gsl_matrix_free(u1);	
};

void gcm::AnisotropicMatrix3D::createAy(initializer_list<gcm_real> params)
{
	clear();
	
	gcm_real rho;
	AnisotropicNumbers numbers;
	getRheologyParameters(params, numbers, rho);
	
	gsl_matrix* a = gsl_matrix_alloc (9, 9);
	clear(a);
		
	gsl_matrix_set (a, 0, 8, -1/rho);
	gsl_matrix_set (a, 1, 4, -1/rho);
	gsl_matrix_set (a, 2, 6, -1/rho);
	
	gsl_matrix_set (a, 3, 0, -numbers.c[5]);
	gsl_matrix_set (a, 3, 1, -numbers.c[1]);
	gsl_matrix_set (a, 3, 2, -numbers.c[3]);
	
	gsl_matrix_set (a, 4, 0, -numbers.c[10]);
	gsl_matrix_set (a, 4, 1, -numbers.c[6]);
	gsl_matrix_set (a, 4, 2, -numbers.c[8]);
	
	gsl_matrix_set (a, 5, 0, -numbers.c[14]);
	gsl_matrix_set (a, 5, 1, -numbers.c[7]);
	gsl_matrix_set (a, 5, 2, -numbers.c[12]);
	
	gsl_matrix_set (a, 6, 0, -numbers.c[17]);
	gsl_matrix_set (a, 6, 1, -numbers.c[8]);
	gsl_matrix_set (a, 6, 2, -numbers.c[15]);
	
	gsl_matrix_set (a, 7, 0, -numbers.c[19]);
	gsl_matrix_set (a, 7, 1, -numbers.c[9]);
	gsl_matrix_set (a, 7, 2, -numbers.c[16]);

	gsl_matrix_set (a, 8, 0, -numbers.c[20]);
	gsl_matrix_set (a, 8, 1, -numbers.c[10]);
	gsl_matrix_set (a, 8, 2, -numbers.c[17]);
	
	gsl_matrix* u1 = gsl_matrix_alloc (9, 9);
	gsl_matrix* l = gsl_matrix_alloc (9, 9);
	gsl_matrix* u = gsl_matrix_alloc (9, 9);
	
	decompositeIt(a, u1, l, u);
	
	gslTogcm(a, A);
	gslTogcm(u, U);
	gslTogcm(l, L);
	gslTogcm(u1, U1);

	gsl_matrix_free(a);	
	gsl_matrix_free(u);	
	gsl_matrix_free(l);	
	gsl_matrix_free(u1);
};

void gcm::AnisotropicMatrix3D::createAz(initializer_list<gcm_real> params)
{
	clear();
	
	gcm_real rho;
	AnisotropicNumbers numbers;
	getRheologyParameters(params, numbers, rho);
	
	gsl_matrix* a = gsl_matrix_alloc (9, 9);
	clear(a);
		
	gsl_matrix_set (a, 0, 7, -1/rho);
	gsl_matrix_set (a, 1, 6, -1/rho);
	gsl_matrix_set (a, 2, 5, -1/rho);
	
	gsl_matrix_set (a, 3, 0, -numbers.c[4]);
	gsl_matrix_set (a, 3, 1, -numbers.c[3]);
	gsl_matrix_set (a, 3, 2, -numbers.c[2]);
	
	gsl_matrix_set (a, 4, 0, -numbers.c[9]);
	gsl_matrix_set (a, 4, 1, -numbers.c[8]);
	gsl_matrix_set (a, 4, 2, -numbers.c[7]);
	
	gsl_matrix_set (a, 5, 0, -numbers.c[13]);
	gsl_matrix_set (a, 5, 1, -numbers.c[12]);
	gsl_matrix_set (a, 5, 2, -numbers.c[11]);
	
	gsl_matrix_set (a, 6, 0, -numbers.c[16]);
	gsl_matrix_set (a, 6, 1, -numbers.c[15]);
	gsl_matrix_set (a, 6, 2, -numbers.c[12]);
	
	gsl_matrix_set (a, 7, 0, -numbers.c[18]);
	gsl_matrix_set (a, 7, 1, -numbers.c[16]);
	gsl_matrix_set (a, 7, 2, -numbers.c[13]);
	
	gsl_matrix_set (a, 8, 0, -numbers.c[19]);
	gsl_matrix_set (a, 8, 1, -numbers.c[17]);
	gsl_matrix_set (a, 8, 2, -numbers.c[14]);
	
	gsl_matrix* u1 = gsl_matrix_alloc (9, 9);
	gsl_matrix* l = gsl_matrix_alloc (9, 9);
	gsl_matrix* u = gsl_matrix_alloc (9, 9);
	
	decompositeIt(a, u1, l, u);
	
	gslTogcm(a, A);
	gslTogcm(u, U);
	gslTogcm(l, L);
	gslTogcm(u1, U1);
	
	gsl_matrix_free(a);	
	gsl_matrix_free(u);	
	gsl_matrix_free(l);	
	gsl_matrix_free(u1);
};

