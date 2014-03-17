#include <time.h>
#include <gtest/gtest.h>

#include "materials/IAnisotropicElasticMaterial.h"
#include "materials/AnisotropicElasticMaterial.h"
#include "materials/IsotropicElasticMaterial.h"
#include "util/AnisotropicMatrix3DAnalytical.h"
#include "util/AnisotropicMatrix3D.h"
#include "util/ElasticMatrix3D.h"

#define ITERATIONS 1000

// Use these limits if anisotropic rheology parameters tensor should be 
// isotropic one plus smaller random values
#define LAMBDA_LIMIT 0.0
#define MU_LIMIT 0.0
// Random part of anisotropic rheology parameters tensor
#define RANDOM_LIMIT 1.0e+3

#define RHO_LIMIT 10.0

// Limits for isotropic transition test
#define ISOTROPIC_LAMBDA_LIMIT 1.0e+9
#define ISOTROPIC_MU_LIMIT 1.0e+8
#define ISOTROPIC_RHO_LIMIT 1.0e+4

AnisotropicElasticMaterial generateRandomMaterial(string name)
{
    gcm_real la = LAMBDA_LIMIT * (double) rand() / RAND_MAX;
    gcm_real mu = MU_LIMIT * (double) rand() / RAND_MAX;
    
    float L[6][6];
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < i; j++)
            L[i][j] = 0;
            
        L[i][i] = RANDOM_LIMIT * (double) rand() / RAND_MAX;
        for (int j = i+1; j < 6; j++)
            L[i][j] = RANDOM_LIMIT * (double) rand() / RAND_MAX;
    }
    
    float matC[6][6];
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            matC[i][j] = 0;
            for (int k = 0; k < 6; k++)
                matC[i][j] += L[k][i] * L[k][j];
        }
    }
    
    IAnisotropicElasticMaterial::RheologyParameters C;
    C.c11 = la + 2 * mu + matC[0][0];
    C.c12 = la + matC[0][1];
    C.c13 = la + matC[0][2];
    C.c14 = matC[0][3];
    C.c15 = matC[0][4];
    C.c16 = matC[0][5];
    C.c22 = la + 2 * mu + matC[1][1];
    C.c23 = la + matC[1][2];
    C.c24 = matC[1][3];
    C.c25 = matC[1][4];
    C.c26 = matC[1][5];
    C.c33 = la + 2 * mu + matC[2][2];
    C.c34 = matC[2][3];
    C.c35 = matC[2][4];
    C.c36 = matC[2][5];
    C.c44 = mu + matC[3][3];
    C.c45 = matC[3][4];
    C.c46 = matC[3][5];
    C.c55 = mu + matC[4][4];
    C.c56 = matC[4][5];
    C.c66 = mu + matC[5][5];
    
    float rho = RHO_LIMIT * (double) rand() / RAND_MAX;
    gcm_real crackThreshold = numeric_limits<gcm_real>::infinity();
    
    AnisotropicElasticMaterial mat(name, rho, crackThreshold, C);
    return mat;
};

AnisotropicElasticMaterial generateOrthotropicMaterial(string name)
{
    gcm_real la = ISOTROPIC_LAMBDA_LIMIT * (double) rand() / RAND_MAX;
    gcm_real mu = ISOTROPIC_MU_LIMIT * (double) rand() / RAND_MAX;
    float pxxNorm = 0.1 + 0.9 * (double) rand() / RAND_MAX;
    float pyyNorm = 0.1 + 0.9 * (double) rand() / RAND_MAX;
    float pzzNorm = 0.1 + 0.9 * (double) rand() / RAND_MAX;
    float pxyNorm = 0.1 + 0.9 * (double) rand() / RAND_MAX;
    float pxzNorm = 0.1 + 0.9 * (double) rand() / RAND_MAX;
    float pyzNorm = 0.1 + 0.9 * (double) rand() / RAND_MAX;
    float sxyNorm = 0.1 + 0.9 * (double) rand() / RAND_MAX;
    float sxzNorm = 0.1 + 0.9 * (double) rand() / RAND_MAX;
    float syzNorm = 0.1 + 0.9 * (double) rand() / RAND_MAX;

    IAnisotropicElasticMaterial::RheologyParameters C;
    C.c11 = (la + 2 * mu) * pxxNorm;
    C.c12 = la * pxyNorm;
    C.c13 = la * pxzNorm;
    C.c14 = 0;
    C.c15 = 0;
    C.c16 = 0;
    C.c22 = (la + 2 * mu) * pyyNorm;
    C.c23 = la * pyzNorm;
    C.c24 = 0;
    C.c25 = 0;
    C.c26 = 0;
    C.c33 = (la + 2 * mu) * pzzNorm;
    C.c34 = 0;
    C.c35 = 0;
    C.c36 = 0;
    C.c44 = mu * sxyNorm;
    C.c45 = 0;
    C.c46 = 0;
    C.c55 = mu * sxzNorm;
    C.c56 = 0;
    C.c66 = mu * syzNorm;
    
    float rho = RHO_LIMIT * (double) rand() / RAND_MAX;
    gcm_real crackThreshold = numeric_limits<gcm_real>::infinity();
    
    AnisotropicElasticMaterial mat(name, rho, crackThreshold, C);
    return mat;
};

void testDecomposition(RheologyMatrix3D& matrix, AnisotropicElasticMaterial(*generateMaterial)(string))
{
    CalcNode anisotropicNode;
    
    for (int count = 0; count < ITERATIONS; count++) {
        
        string testMaterialName = "AnisotropicMatrix3D_FuzzyMultiplication_" + count;
        AnisotropicElasticMaterial mat = generateMaterial(testMaterialName);
        anisotropicNode.setMaterialId(Engine::getInstance().addMaterial(&mat));
        
        matrix.refresh();
        for (int i = 0; i < 3; i++) {
            switch (i) {
            case 0: matrix.createAx(anisotropicNode);
                break;
            case 1: matrix.createAy(anisotropicNode);
                break;
            case 2: matrix.createAz(anisotropicNode);
                break;
            }
            
            // Test decomposition
            ASSERT_TRUE( matrix.getU1() * matrix.getL() * matrix.getU() == matrix.getA() );
            // Test eigen values and eigen rows
            ASSERT_TRUE( matrix.getU1() * matrix.getL() == matrix.getA() * matrix.getU1() );
        }
        Engine::getInstance().clear();
    }
};

void testIsotropicTransition(RheologyMatrix3D& anisotropicMatrix)
{
    for (int count = 0; count < ITERATIONS; count++) {
        gcm_real la = ISOTROPIC_LAMBDA_LIMIT * (double) rand() / RAND_MAX;
        gcm_real mu = ISOTROPIC_MU_LIMIT * (double) rand() / RAND_MAX;
        gcm_real rho = ISOTROPIC_RHO_LIMIT * (double) rand() / RAND_MAX;
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

        RheologyMatrix3D& isotropicMatrix = isotropicNode.getRheologyMatrix();

        anisotropicMatrix.refresh();
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
        Engine::getInstance().clear();
    }
};

TEST(AnisotropicMatrix3D, AnalyticalFuzzRandom) 
{
    srand(time(NULL));
    AnisotropicMatrix3DAnalytical analyticalMatrix;
    testDecomposition(analyticalMatrix, generateRandomMaterial);
};

TEST(AnisotropicMatrix3D, NumericalFuzzRandom) 
{
    srand(time(NULL));
    AnisotropicMatrix3D numericalMatrix;
    testDecomposition(numericalMatrix, generateRandomMaterial);
};

TEST(AnisotropicMatrix3D, AnalyticalFuzzOrthotropic) 
{
    srand(time(NULL));
    AnisotropicMatrix3DAnalytical analyticalMatrix;
    testDecomposition(analyticalMatrix, generateOrthotropicMaterial);
};

TEST(AnisotropicMatrix3D, NumericalFuzzOrthotropic) 
{
    srand(time(NULL));
    AnisotropicMatrix3D numericalMatrix;
    testDecomposition(numericalMatrix, generateOrthotropicMaterial);
};

TEST(AnisotropicMatrix3D, AnalyticalIsotropicTransition)
{
    srand(time(NULL));
    AnisotropicMatrix3DAnalytical analyticalMatrix;
    testIsotropicTransition(analyticalMatrix);
};

TEST(AnisotropicMatrix3D, NumericalIsotropicTransition) 
{
    srand(time(NULL));
    AnisotropicMatrix3D numericalMatrix;
    testIsotropicTransition(numericalMatrix);
};

TEST(AnisotropicMatrix3D, AnalyticalEqNumerical) 
{
    AnisotropicMatrix3DAnalytical analyticalMatrix;
    AnisotropicMatrix3D numericalMatrix;
    CalcNode anisotropicNode;
    
    string testMaterialName = "AnisotropicMatrix3D_AnalyticalEqNumerical";
    AnisotropicElasticMaterial mat = generateRandomMaterial(testMaterialName);
    anisotropicNode.setMaterialId(Engine::getInstance().addMaterial(&mat));
    
    analyticalMatrix.createAx(anisotropicNode);
    numericalMatrix.createAx(anisotropicNode);
    ASSERT_TRUE( analyticalMatrix.getA() == numericalMatrix.getA() );
    
    analyticalMatrix.createAy(anisotropicNode);
    numericalMatrix.createAy(anisotropicNode);
    ASSERT_TRUE( analyticalMatrix.getA() == numericalMatrix.getA() );
    
    analyticalMatrix.createAz(anisotropicNode);
    numericalMatrix.createAz(anisotropicNode);
    ASSERT_TRUE( analyticalMatrix.getA() == numericalMatrix.getA() );
};

TEST(AnisotropicMatrix3D, AnalyticalVsNumericalPerf) 
{
    float MINIMAL_EXPECTED_SPEEDUP = 2.0;
    
    struct timespec start;
    struct timespec end;
    long analyticalTime;
    long numericalTime;
    
    AnisotropicMatrix3DAnalytical analyticalMatrix;
    AnisotropicMatrix3D numericalMatrix;
    CalcNode anisotropicNode;
    
    string testMaterialName = "AnisotropicMatrix3D_Perf";
    AnisotropicElasticMaterial mat = generateRandomMaterial(testMaterialName);
    anisotropicNode.setMaterialId(Engine::getInstance().addMaterial(&mat));
    
    clock_gettime(CLOCK_REALTIME, &start);
    for (int count = 0; count < ITERATIONS; count++) {
        
            analyticalMatrix.refresh();
            for (int i = 0; i < 3; i++) {
                switch (i) {
                case 0: analyticalMatrix.createAx(anisotropicNode);
                    break;
                case 1: analyticalMatrix.createAy(anisotropicNode);
                    break;
                case 2: analyticalMatrix.createAz(anisotropicNode);
                    break;
                }
            }
    }
    clock_gettime(CLOCK_REALTIME, &end);
    analyticalTime = (end.tv_sec - start.tv_sec) * 1.0e3 + (end.tv_nsec - start.tv_nsec) / 1.0e6;
    
    clock_gettime(CLOCK_REALTIME, &start);
    for (int count = 0; count < ITERATIONS; count++) {
        
            numericalMatrix.refresh();
            for (int i = 0; i < 3; i++) {
                switch (i) {
                case 0: numericalMatrix.createAx(anisotropicNode);
                    break;
                case 1: numericalMatrix.createAy(anisotropicNode);
                    break;
                case 2: numericalMatrix.createAz(anisotropicNode);
                    break;
                }
            }
    }
    clock_gettime(CLOCK_REALTIME, &end);
    numericalTime = (end.tv_sec - start.tv_sec) * 1.0e3 + (end.tv_nsec - start.tv_nsec) / 1.0e6;
    
    ASSERT_TRUE( (float)numericalTime / (float)analyticalTime > MINIMAL_EXPECTED_SPEEDUP );
};
