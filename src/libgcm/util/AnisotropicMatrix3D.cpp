#include "libgcm/util/AnisotropicMatrix3D.hpp"
#include "libgcm/materials/AnisotropicElasticMaterial.hpp"

gcm::AnisotropicMatrix3D::AnisotropicMatrix3D(): ImmutableRheologyMatrix3D()
{

}

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

void gcm::AnisotropicMatrix3D::realChecker(gsl_matrix_complex* a, gsl_matrix* u, int stage)
{
    int i, j;
    gsl_complex z;

    switch(stage) {
    case 0:
        for (i = 0; i < 9; i++)
            for (j = 0; j < 6; j++) {
                z = gsl_matrix_complex_get(a, i, j);
                if (fabs(GSL_IMAG(z)) > 1e-8) THROW_INVALID_INPUT("Vector is complex!");
                gsl_matrix_set(u, i, j, GSL_REAL(z));
            }

        for (i = 0; i < 9; i++)
            for (j = 6; j < 9; j++)
                gsl_matrix_set(u, i, j, 0.0);

        gsl_matrix_set(u, 6, 6, 1.0);
        gsl_matrix_set(u, 7, 7, 1.0);
        gsl_matrix_set(u, 8, 8, 1.0);
        break;

    case 1:
        for (i = 0; i < 9; i++)
            for (j = 0; j < 6; j++) {
                z = gsl_matrix_complex_get(a, i, j);
                if (fabs(GSL_IMAG(z)) > 1e-8) THROW_INVALID_INPUT("Vector is complex!");
                gsl_matrix_set(u, i, j, GSL_REAL(z));
            }

        for (i = 0; i < 9; i++)
            for (j = 6; j < 9; j++)
                gsl_matrix_set(u, i, j, 0.0);

        gsl_matrix_set(u, 3, 6, 1.0);
        gsl_matrix_set(u, 5, 7, 1.0);
        gsl_matrix_set(u, 8, 8, 1.0);
        break;

    case 2:
        for (i = 0; i < 9; i++)
            for (j = 0; j < 6; j++) {
                z = gsl_matrix_complex_get(a, i, j);
                if (fabs(GSL_IMAG(z)) > 1e-8) THROW_INVALID_INPUT("Vector is complex!");
                gsl_matrix_set(u, i, j, GSL_REAL(z));
            }

        for (i = 0; i < 9; i++)
            for (j = 6; j < 9; j++)
                gsl_matrix_set(u, i, j, 0.0);

        gsl_matrix_set(u, 3, 6, 1.0);
        gsl_matrix_set(u, 4, 7, 1.0);
        gsl_matrix_set(u, 6, 8, 1.0);
        break;
    }
};

void gcm::AnisotropicMatrix3D::realChecker(gsl_vector_complex* a, gsl_matrix* l)
{
    gsl_complex z;

    for (int i = 0; i < 9; i++) {
        z = gsl_vector_complex_get(a, i);
        if (fabs(GSL_IMAG(z)) > 1e-8) THROW_INVALID_INPUT("Value is complex!");
        gsl_matrix_set(l, i, i, GSL_REAL(z));
    }
};

void gcm::AnisotropicMatrix3D::decompositeIt(gsl_matrix* a, gsl_vector_complex* eval,  gsl_matrix_complex* evec)
{
    // Finding eigenvalues & eigenvectors of 'a'
    gsl_eigen_nonsymmv_workspace* w = gsl_eigen_nonsymmv_alloc(9);
    gsl_eigen_nonsymmv(a, eval, evec, w);
    gsl_eigen_nonsymmv_free(w);
    gsl_eigen_nonsymmv_sort(eval, evec, GSL_EIGEN_SORT_ABS_DESC);
};

void gcm::AnisotropicMatrix3D::initializeAx(const Material* material, gcm_matrix& A, gcm_matrix& L, gcm_matrix& U, gcm_matrix& U1)
{
    A.clear();
    L.clear();
    U.clear();
    U1.clear();


    gsl_matrix* a = gsl_matrix_alloc(9, 9);
    clear(a);

#ifdef NDEBUG
    const auto mat = static_cast<const AnisotropicElasticMaterial*> (material);
#else
    const auto mat = dynamic_cast<const AnisotropicElasticMaterial*> (material);
    assert_true(mat);
#endif

    auto rho = mat->getRho();
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

    gslTogcm(a, A);

    gsl_vector_complex* eval = gsl_vector_complex_alloc(9);
    gsl_matrix_complex* evec = gsl_matrix_complex_alloc(9, 9);
    decompositeIt(a, eval, evec);

    // Checking eigenvalues & filling L
    gsl_matrix* l = gsl_matrix_alloc(9, 9);
    clear(l);
    realChecker(eval, l);
    gslTogcm(l, L);

    // Checking eigenvectors & filling U1
    gsl_matrix* u1 = gsl_matrix_alloc(9, 9);
    clear(u1);
    realChecker(evec, u1, 0);
    gslTogcm(u1, U1);

    // Inverting of U1 matrix
    int i;
    gsl_matrix* u = gsl_matrix_alloc(9, 9);
    gsl_permutation* perm = gsl_permutation_alloc(9);
    gsl_linalg_LU_decomp(u1, perm, &i);
    gsl_linalg_LU_invert(u1, perm, u);

    gslTogcm(u, U);

    gsl_matrix_free(a);
    gsl_matrix_free(u);
    gsl_matrix_free(l);
    gsl_matrix_free(u1);
    gsl_vector_complex_free(eval);
    gsl_matrix_complex_free(evec);
    gsl_permutation_free(perm);
};

void gcm::AnisotropicMatrix3D::initializeAy(const Material* material, gcm_matrix& A, gcm_matrix& L, gcm_matrix& U, gcm_matrix& U1)
{
    A.clear();
    L.clear();
    U.clear();
    U1.clear();

    gsl_matrix* a = gsl_matrix_alloc(9, 9);
    clear(a);

#ifdef NDEBUG
    const auto mat = static_cast<const AnisotropicElasticMaterial*> (material);
#else
    const auto mat = dynamic_cast<const AnisotropicElasticMaterial*> (material);
    assert_true(mat);
#endif

    auto rho = mat->getRho();
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

    gslTogcm(a, A);

    gsl_vector_complex* eval = gsl_vector_complex_alloc(9);
    gsl_matrix_complex* evec = gsl_matrix_complex_alloc(9, 9);
    decompositeIt(a, eval, evec);

    // Checking eigenvalues & filling L
    gsl_matrix* l = gsl_matrix_alloc(9, 9);
    clear(l);
    realChecker(eval, l);
    gslTogcm(l, L);

    // Checking eigenvalues & filling U1
    gsl_matrix* u1 = gsl_matrix_alloc(9, 9);
    clear(u1);
    realChecker(evec, u1, 1);
    gslTogcm(u1, U1);

    // Inverting of U1 matrix
    int i;
    gsl_matrix* u = gsl_matrix_alloc(9, 9);
    gsl_permutation* perm = gsl_permutation_alloc(9);
    gsl_linalg_LU_decomp(u1, perm, &i);
    gsl_linalg_LU_invert(u1, perm, u);
    gslTogcm(u, U);

    gsl_matrix_free(a);
    gsl_matrix_free(u);
    gsl_matrix_free(l);
    gsl_matrix_free(u1);
    gsl_vector_complex_free(eval);
    gsl_matrix_complex_free(evec);
    gsl_permutation_free(perm);
};

void gcm::AnisotropicMatrix3D::initializeAz(const Material* material, gcm_matrix& A, gcm_matrix& L, gcm_matrix& U, gcm_matrix& U1)
{
    A.clear();
    L.clear();
    U.clear();
    U1.clear();

    gsl_matrix* a = gsl_matrix_alloc(9, 9);
    clear(a);

#ifdef NDEBUG
    const auto mat = static_cast<const AnisotropicElasticMaterial*> (material);
#else
    const auto mat = dynamic_cast<const AnisotropicElasticMaterial*> (material);
    assert_true(mat);
#endif

    auto rho = mat->getRho();
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

    gslTogcm(a, A);

    gsl_vector_complex* eval = gsl_vector_complex_alloc(9);
    gsl_matrix_complex* evec = gsl_matrix_complex_alloc(9, 9);
    decompositeIt(a, eval, evec);

    // Checking eigenvalues & filling L
    gsl_matrix* l = gsl_matrix_alloc(9, 9);
    clear(l);
    realChecker(eval, l);
    gslTogcm(l, L);

    // Checking eigenvalues & filling U1
    gsl_matrix* u1 = gsl_matrix_alloc(9, 9);
    clear(u1);
    realChecker(evec, u1, 2);
    gslTogcm(u1, U1);

    // Inverting of U1 matrix
    int i;
    gsl_matrix* u = gsl_matrix_alloc(9, 9);
    gsl_permutation* perm = gsl_permutation_alloc(9);
    gsl_linalg_LU_decomp(u1, perm, &i);
    gsl_linalg_LU_invert(u1, perm, u);
    gslTogcm(u, U);

    gsl_matrix_free(a);
    gsl_matrix_free(u);
    gsl_matrix_free(l);
    gsl_matrix_free(u1);
    gsl_vector_complex_free(eval);
    gsl_matrix_complex_free(evec);
    gsl_permutation_free(perm);
};

