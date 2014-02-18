#include "util/AnisotropicMatrix3D.h"

gcm::AnisotropicMatrix3D::AnisotropicMatrix3D()
{
	zero_all();
};

gcm::AnisotropicMatrix3D::~AnisotropicMatrix3D() { };

void gcm::AnisotropicMatrix3D::prepare_matrix(const AnisotropicNumbers &numbers, float rho, int stage)
{
	assert (rho > 0);
	for(int i = 0; i < 21; i++)
		assert(numbers.c[i] > 0);

	if (stage == 0) {
		CreateAx(numbers, rho);
	} else if (stage == 1) { 
		CreateAy(numbers, rho); 
	} else if (stage == 2) { 
		CreateAz(numbers, rho); 
	} else {
		THROW_BAD_CONFIG("Wrong stage number");
	}
};

float gcm::AnisotropicMatrix3D::max_lambda()
{
	float res = 0;
	for (unsigned i = 0; i < GCM_MATRIX_SIZE; ++i)
		if(L(i,i) > res)
			res = L(i,i);
	return res;
};

void gcm::AnisotropicMatrix3D::GslToGcm(gsl_matrix* a, gcm_matrix& b) {
	for(int i = 0; i < 9; i++)
		for(int j = 0; j < 9; j++)
			b(i, j) = gsl_matrix_get(a, i, j);
};

void gcm::AnisotropicMatrix3D::GcmToGsl(const gcm_matrix& a, gsl_matrix* b) {
	for(int i = 0; i < 9; i++)
		for(int j = 0; j < 9; j++)
			gsl_matrix_set(b, i, j, a.p[i][j]);
};

void gcm::AnisotropicMatrix3D::setZero(gcm_matrix& a)
{
	for(int i = 0; i < 9; i++)
		for(int j = 0; j < 9; j++)
			a(i, j) = 0.0;
};

void gcm::AnisotropicMatrix3D::setZero(gsl_matrix* a)
{
	for(int i = 0; i < 9; i++)
		for(int j = 0; j < 9; j++)
			gsl_matrix_set (a, i, j, 0);
};

void gcm::AnisotropicMatrix3D::zero_all()
{
	A.clear();
	L.clear();
	U.clear();
	U1.clear();
};

void gcm::AnisotropicMatrix3D::DecompositeIt(gsl_matrix* a, gsl_matrix_complex* u1, gsl_matrix_complex* l, gsl_matrix_complex* u) 
{
	int i, j;
	
	gsl_matrix* tmp = gsl_matrix_alloc(9, 9);
	
	for(i = 0; i < 9; i++)
		for(j = 0; j < 9; j++)
			gsl_matrix_set(tmp, i, j, gsl_matrix_get(a, i, j));
	
	// Finding eigenvalues & eigenvectors of 'a'
	gsl_vector_complex* eval = gsl_vector_complex_alloc (9);
	gsl_eigen_nonsymmv_workspace* w = gsl_eigen_nonsymmv_alloc (9);
	gsl_eigen_nonsymmv (tmp, eval, u, w);
	gsl_eigen_nonsymmv_free (w);
	gsl_eigen_nonsymmv_sort (eval, u, GSL_EIGEN_SORT_ABS_DESC);
	
	for(i = 0; i < 9; i++)
		for(j = 0; j < 9; j++)
			GSL_SET_COMPLEX(gsl_matrix_complex_ptr(l, i, j), 0, 0);
			
	for(i = 0; i < 9; i++)
		gsl_matrix_complex_set (l, i, i, gsl_vector_complex_get (eval, i));
			
	// Inverting of 'u'
	gsl_permutation* perm = gsl_permutation_alloc (9);
	gsl_linalg_complex_LU_decomp (u, perm, &j);
	gsl_linalg_complex_LU_invert (u, perm, u1);
	
	gsl_matrix_free (tmp);
		
};

void gcm::AnisotropicMatrix3D::CreateAx(const AnisotropicNumbers &numbers, float rho)
{
	gsl_matrix* a = gsl_matrix_alloc (9, 9);
	setZero(a);
		
	gsl_matrix_set (a, 0, 9, -1/rho);
	gsl_matrix_set (a, 1, 4, -1/rho);
	gsl_matrix_set (a, 2, 6, -1/rho);
	gsl_matrix_set (a, 3, 0, -numbers.c[0]);
	gsl_matrix_set (a, 3, 1, -numbers.c[5]);
	gsl_matrix_set (a, 3, 0, -numbers.c[4]);
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
	
	gsl_matrix_complex* u1 = gsl_matrix_complex_alloc (9, 9);
	gsl_matrix_complex* l = gsl_matrix_complex_alloc (9, 9);
	gsl_matrix_complex* u = gsl_matrix_complex_alloc (9, 9);
	
	DecompositeIt(a, u1, l, u);
	
		
};

void gcm::AnisotropicMatrix3D::CreateAy(const AnisotropicNumbers &numbers, float rho)
{
	zero_all();


};

void gcm::AnisotropicMatrix3D::CreateAz(const AnisotropicNumbers &numbers, float rho)
{
	zero_all();


};



