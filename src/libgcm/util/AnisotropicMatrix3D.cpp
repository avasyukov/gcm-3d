#include "util/AnisotropicMatrix3D.h"
#include "materials/AnisotropicElasticMaterial.h"

void gcm::AnisotropicMatrix3D::clear()
{
    A.clear();
    L.clear();
    U.clear();
    U1.clear();
};

void gcm::AnisotropicMatrix3D::gslTogcm(gsl_matrix* a, gcm_matrix& b)
{
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            b(i, j) = gsl_matrix_get(a, i, j);
};

void gcm::AnisotropicMatrix3D::gcmTogsl(const gcm_matrix& a, gsl_matrix* b)
{
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            gsl_matrix_set(b, i, j, a.p[i][j]);
};

void gcm::AnisotropicMatrix3D::clear(gsl_matrix* a)
{
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            gsl_matrix_set(a, i, j, 0);
};

void gcm::AnisotropicMatrix3D::realChecker(gsl_matrix_complex* a, gsl_matrix* u)
{
    int i, j;
    gsl_complex z;

    for (i = 0; i < 9; i++)
        for (j = 0; j < 9; j++) {
            z = gsl_matrix_complex_get(a, i, j);
            if (fabs(GSL_IMAG(z)) > 1e-8) THROW_INVALID_INPUT("Vector is complex!");
            gsl_matrix_set(u, i, j, GSL_REAL(z));
        }

};

void gcm::AnisotropicMatrix3D::realChecker(gsl_vector_complex* a, gsl_matrix* l)
{
    int i;
    gsl_complex z;

    for (i = 0; i < 9; i++) {
        z = gsl_vector_complex_get(a, i);
        if (fabs(GSL_IMAG(z)) > 1e-8) THROW_INVALID_INPUT("Value is complex!");
        gsl_matrix_set(l, i, i, GSL_REAL(z));
    }
};

void gcm::AnisotropicMatrix3D::decompositeIt(gsl_matrix* a, gsl_matrix* u, gsl_matrix* l, gsl_matrix* u1)
{
    int i;

    gsl_matrix* temp = gsl_matrix_alloc(9, 9);
    gsl_matrix_memcpy(temp, a);

    // Finding eigenvalues & eigenvectors of 'a'
    gsl_vector_complex* eval = gsl_vector_complex_alloc(9);
    gsl_matrix_complex* evec = gsl_matrix_complex_alloc(9, 9);
    gsl_eigen_nonsymmv_workspace* w = gsl_eigen_nonsymmv_alloc(9);
    gsl_eigen_nonsymmv(temp, eval, evec, w);
    gsl_eigen_nonsymmv_free(w);

    // eval & evec must be real
    clear(l);
    realChecker(eval, l);
    realChecker(evec, u);

    gsl_permutation* perm = gsl_permutation_alloc(9);
    gsl_matrix_memcpy(temp, u);

    // Inverting of G matrix
    gsl_linalg_LU_decomp(temp, perm, &i);
    gsl_linalg_LU_invert(temp, perm, u1);

    gsl_matrix_free(temp);
    gsl_vector_complex_free(eval);
    gsl_matrix_complex_free(evec);

};

void gcm::AnisotropicMatrix3D::createAx(const ICalcNode& node)
{
    clear();


    gsl_matrix* a = gsl_matrix_alloc(9, 9);
    clear(a);

#ifdef NDEBUG
    AnisotropicElasticMaterial* mat = static_cast<AnisotropicElasticMaterial*> (node.getMaterial());
#else
    AnisotropicElasticMaterial* mat = dynamic_cast<AnisotropicElasticMaterial*> (node.getMaterial());
    assert(mat);
#endif

    auto rho = node.getRho();
    auto params = mat->getParameters();


    gsl_matrix_set(a, 0, 3, -1 / rho);
    gsl_matrix_set(a, 1, 4, -1 / rho);
    gsl_matrix_set(a, 2, 5, -1 / rho);

    gsl_matrix_set(a, 3, 0, -params.c11);
    gsl_matrix_set(a, 3, 1, -params.c16);
    gsl_matrix_set(a, 3, 2, -params.c15);

    gsl_matrix_set(a, 4, 0, -params.c16);
    gsl_matrix_set(a, 4, 1, -params.c66);
    gsl_matrix_set(a, 4, 2, -params.c56);

    gsl_matrix_set(a, 5, 0, -params.c15);
    gsl_matrix_set(a, 5, 1, -params.c56);
    gsl_matrix_set(a, 5, 2, -params.c55);

    gsl_matrix_set(a, 6, 0, -params.c12);
    gsl_matrix_set(a, 6, 1, -params.c26);
    gsl_matrix_set(a, 6, 2, -params.c25);

    gsl_matrix_set(a, 7, 0, -params.c14);
    gsl_matrix_set(a, 7, 1, -params.c46);
    gsl_matrix_set(a, 7, 2, -params.c45);

    gsl_matrix_set(a, 8, 0, -params.c13);
    gsl_matrix_set(a, 8, 1, -params.c36);
    gsl_matrix_set(a, 8, 2, -params.c35);

    gsl_matrix* u1 = gsl_matrix_alloc(9, 9);
    gsl_matrix* l = gsl_matrix_alloc(9, 9);
    gsl_matrix* u = gsl_matrix_alloc(9, 9);

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

void gcm::AnisotropicMatrix3D::createAy(const ICalcNode& node)
{
    clear();

    gsl_matrix* a = gsl_matrix_alloc(9, 9);
    clear(a);

#ifdef NDEBUG
    AnisotropicElasticMaterial* mat = static_cast<AnisotropicElasticMaterial*> (node.getMaterial());
#else
    AnisotropicElasticMaterial* mat = dynamic_cast<AnisotropicElasticMaterial*> (node.getMaterial());
    assert(mat);
#endif

    auto rho = node.getRho();
    auto params = mat->getParameters();    
    
    gsl_matrix_set(a, 0, 4, -1 / rho);
    gsl_matrix_set(a, 1, 6, -1 / rho);
    gsl_matrix_set(a, 2, 7, -1 / rho);

    gsl_matrix_set(a, 3, 0, -params.c16);
    gsl_matrix_set(a, 3, 1, -params.c12);
    gsl_matrix_set(a, 3, 2, -params.c14);

    gsl_matrix_set(a, 4, 0, -params.c66);
    gsl_matrix_set(a, 4, 1, -params.c26);
    gsl_matrix_set(a, 4, 2, -params.c46);

    gsl_matrix_set(a, 5, 0, -params.c56);
    gsl_matrix_set(a, 5, 1, -params.c25);
    gsl_matrix_set(a, 5, 2, -params.c45);

    gsl_matrix_set(a, 6, 0, -params.c26);
    gsl_matrix_set(a, 6, 1, -params.c22);
    gsl_matrix_set(a, 6, 2, -params.c24);

    gsl_matrix_set(a, 7, 0, -params.c46);
    gsl_matrix_set(a, 7, 1, -params.c24);
    gsl_matrix_set(a, 7, 2, -params.c44);

    gsl_matrix_set(a, 8, 0, -params.c36);
    gsl_matrix_set(a, 8, 1, -params.c23);
    gsl_matrix_set(a, 8, 2, -params.c34);

    gsl_matrix* u1 = gsl_matrix_alloc(9, 9);
    gsl_matrix* l = gsl_matrix_alloc(9, 9);
    gsl_matrix* u = gsl_matrix_alloc(9, 9);

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

void gcm::AnisotropicMatrix3D::createAz(const ICalcNode& node)
{
    clear();

    gsl_matrix* a = gsl_matrix_alloc(9, 9);
    clear(a);
    
#ifdef NDEBUG
    AnisotropicElasticMaterial* mat = static_cast<AnisotropicElasticMaterial*> (node.getMaterial());
#else
    AnisotropicElasticMaterial* mat = dynamic_cast<AnisotropicElasticMaterial*> (node.getMaterial());
    assert(mat);
#endif

    auto rho = node.getRho();
    auto params = mat->getParameters();    

    gsl_matrix_set(a, 0, 5, -1 / rho);
    gsl_matrix_set(a, 1, 7, -1 / rho);
    gsl_matrix_set(a, 2, 8, -1 / rho);

    gsl_matrix_set(a, 3, 0, -params.c15);
    gsl_matrix_set(a, 3, 1, -params.c14);
    gsl_matrix_set(a, 3, 2, -params.c13);

    gsl_matrix_set(a, 4, 0, -params.c56);
    gsl_matrix_set(a, 4, 1, -params.c46);
    gsl_matrix_set(a, 4, 2, -params.c36);

    gsl_matrix_set(a, 5, 0, -params.c55);
    gsl_matrix_set(a, 5, 1, -params.c45);
    gsl_matrix_set(a, 5, 2, -params.c35);

    gsl_matrix_set(a, 6, 0, -params.c25);
    gsl_matrix_set(a, 6, 1, -params.c24);
    gsl_matrix_set(a, 6, 2, -params.c23);

    gsl_matrix_set(a, 7, 0, -params.c45);
    gsl_matrix_set(a, 7, 1, -params.c44);
    gsl_matrix_set(a, 7, 2, -params.c34);

    gsl_matrix_set(a, 8, 0, -params.c35);
    gsl_matrix_set(a, 8, 1, -params.c34);
    gsl_matrix_set(a, 8, 2, -params.c33);

    gsl_matrix* u1 = gsl_matrix_alloc(9, 9);
    gsl_matrix* l = gsl_matrix_alloc(9, 9);
    gsl_matrix* u = gsl_matrix_alloc(9, 9);

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

