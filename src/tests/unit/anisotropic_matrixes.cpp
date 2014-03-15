#include <time.h>


#include "materials/IAnisotropicElasticMaterial.h"
#include "materials/AnisotropicElasticMaterial.h"
#include "materials/IsotropicElasticMaterial.h"
#include "util/AnisotropicMatrix3D.h"
#include "util/ElasticMatrix3D.h"
#include "Engine.h"

#include <gtest/gtest.h>

#define ITERATIONS 1000

using namespace gcm;

class AnisotropicMatrix3DWrapper : public AnisotropicMatrix3D
{
public:

    void decomposite(gsl_matrix* a, gsl_matrix* u, gsl_matrix* l, gsl_matrix* u1)
    {
        decompositeIt(a, u, l, u1);
    }
};

TEST(AnisotropicMatrix3D, FuzzyMultiplication)
{
    srand(time(NULL));

    AnisotropicMatrix3DWrapper m;

    gsl_matrix* a = gsl_matrix_alloc(9, 9);
    gsl_matrix* u1 = gsl_matrix_alloc(9, 9);
    gsl_matrix* l = gsl_matrix_alloc(9, 9);
    gsl_matrix* u = gsl_matrix_alloc(9, 9);
    gsl_matrix* tmp = gsl_matrix_alloc(9, 9);
    double x;
    int i;

    for (i = 0; i < ITERATIONS; i++) {
        for (int j = 0; j < 9; j++)
            for (int k = 0; k <= j; k++) {
                x = pow(10, k)*(double) rand() / RAND_MAX;
                gsl_matrix_set(a, j, k, x);
                gsl_matrix_set(a, k, j, x);
            }
        m.decomposite(a, u1, l, u);

        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, u1, l, 0.0, tmp);
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, tmp, u, 0.0, u1);
        
        for (int j = 0; j < 9; j++)
            for (int k = 0; k < 9; k++)
                ASSERT_NEAR(gsl_matrix_get(a, j, k), gsl_matrix_get(u1, j, k), 1e-6);
            }

    gsl_matrix_free(a);
    gsl_matrix_free(u1);
    gsl_matrix_free(l);
    gsl_matrix_free(u);
    gsl_matrix_free(tmp);
};

TEST(AnisotropicMatrix3D, IsotropicTransition)
{
    srand(time(NULL));

    gcm_real la = 1.0e+9 * (double) rand() / RAND_MAX;
    gcm_real mu = 1.0e+8 * (double) rand() / RAND_MAX;
    gcm_real rho = 1.0e+4 * (double) rand() / RAND_MAX;
    gcm_real crackThreshold = numeric_limits<gcm_real>::infinity();

    CalcNode isotropicNode;
    CalcNode anisotropicNode;

    IAnisotropicElasticMaterial::RheologyParameters params;
    params.c11 = params.c22 = params.c33 = la + 2 * mu;
    params.c44 = params.c55 = params.c66 = mu;
    params.c12 = params.c13 = params.c23 = la;
    params.c14 = params.c15 = params.c16 = 0.0;
    params.c24 = params.c25 = params.c26 = 0.0;
    params.c34 = params.c35 = params.c36 = 0.0;
    params.c45 = params.c46 = params.c56 = 0.0;
    
    IsotropicElasticMaterial m1("AnisotropicMatrix3D_IsotropicTransition_IEM", rho, crackThreshold, la, mu);
    AnisotropicElasticMaterial m2("AnisotropicMatrix3D_IsotropicTransition_AEM", rho, crackThreshold, params);
    
    isotropicNode.setMaterialId(Engine::getInstance().addMaterial(&m1));
    anisotropicNode.setMaterialId(Engine::getInstance().addMaterial(&m2));

    RheologyMatrix3D& anisotropicMatrix = anisotropicNode.getRheologyMatrix();
    RheologyMatrix3D& isotropicMatrix = isotropicNode.getRheologyMatrix();
    
    for (int i = 0; i < 3; i++) {
        switch (i) {
        case 0: isotropicMatrix.createAx(isotropicNode);
            anisotropicMatrix.createAx(anisotropicNode);
            break;
        case 1: isotropicMatrix.createAy(isotropicNode);
            anisotropicMatrix.createAy(anisotropicNode);
            break;
        case 2: isotropicMatrix.createAz(isotropicNode);
            anisotropicMatrix.createAz(anisotropicNode);
            break;
        }

        for (int j = 0; j < 9; j++)
            for (int k = 0; k < 9; k++)
                ASSERT_NEAR(anisotropicMatrix.getA(j, k), isotropicMatrix.getA(j, k), EQUALITY_TOLERANCE);
    }
};
