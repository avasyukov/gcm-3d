#include <cmath>
#include <time.h>
#include <gtest/gtest.h>
#include <functional>
#include <algorithm>

#include "libgcm/Math.hpp"
#include "libgcm/rheology/Material.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/Exception.hpp"
#include "libgcm/rheology/setters/IsotropicRheologyMatrixSetter.hpp"
#include "libgcm/rheology/setters/AnisotropicRheologyMatrixSetter.hpp"
#include "libgcm/rheology/decomposers/IsotropicRheologyMatrixDecomposer.hpp"
#include "libgcm/rheology/decomposers/NumericalRheologyMatrixDecomposer.hpp"
#include "libgcm/rheology/decomposers/AnalyticalRheologyMatrixDecomposer.hpp"
#include "libgcm/rheology/decomposers/AnalyticalRheologyMatrixDecomposer.hpp"

/*
 * Constants used in tests
 */

#define ITERATIONS 1000

#define MAX_ROTATIONS_NUMBER 6

// Limits for random anisotropic rheology parameters tensor
#define RANDOM_C_MIN 0.1
#define RANDOM_C_MAX 1.0

// Limits for random density values
#define RHO_MIN 0.1
#define RHO_MAX 1.0

// Limits for isotropic transition test
#define ISOTROPIC_LAMBDA_MIN 0.1
#define ISOTROPIC_LAMBDA_MAX 1.0
#define ISOTROPIC_MU_MIN 0.1
#define ISOTROPIC_MU_MAX 1.0

/*
 * Helper functions
 */

typedef std::function<MaterialPtr(std::string) > MaterialGenerator;

MaterialPtr generateRandomMaterial(string name)
{
    float matC[6][6];
    // Generate positively defined symmetric matrix matC[][]]
    // using http://en.wikipedia.org/wiki/Cholesky_decomposition
    float L[6][6];
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < i; j++)
            L[i][j] = 0;

        for (int j = i; j < 6; j++)
            L[i][j] = RANDOM_C_MIN + (RANDOM_C_MAX - RANDOM_C_MIN) * (double) rand() / RAND_MAX;
    }

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            matC[i][j] = 0;
            for (int k = 0; k < 6; k++)
                matC[i][j] += L[k][i] * L[k][j];
        }
    }

    Material::RheologyProperties C;
    C.c11 = matC[0][0];
    C.c12 = matC[0][1];
    C.c13 = matC[0][2];
    C.c14 = matC[0][3];
    C.c15 = matC[0][4];
    C.c16 = matC[0][5];
    C.c22 = matC[1][1];
    C.c23 = matC[1][2];
    C.c24 = matC[1][3];
    C.c25 = matC[1][4];
    C.c26 = matC[1][5];
    C.c33 = matC[2][2];
    C.c34 = matC[2][3];
    C.c35 = matC[2][4];
    C.c36 = matC[2][5];
    C.c44 = matC[3][3];
    C.c45 = matC[3][4];
    C.c46 = matC[3][5];
    C.c55 = matC[4][4];
    C.c56 = matC[4][5];
    C.c66 = matC[5][5];

    float rho = RHO_MIN + (RHO_MAX - RHO_MIN) * (double) rand() / RAND_MAX;
    gcm::real crackThreshold = numeric_limits<gcm::real>::infinity();

    return makeMaterialPtr(name, rho, crackThreshold, C);
};

MaterialPtr generateOrthotropicMaterial(string name)
{
    float c11 = RANDOM_C_MIN + (RANDOM_C_MAX - RANDOM_C_MIN) * (double) rand() / RAND_MAX;
    float c12 = RANDOM_C_MIN + (RANDOM_C_MAX - RANDOM_C_MIN) * (double) rand() / RAND_MAX;
    float c13 = RANDOM_C_MIN + (RANDOM_C_MAX - RANDOM_C_MIN) * (double) rand() / RAND_MAX;
    float c22 = RANDOM_C_MIN + (RANDOM_C_MAX - RANDOM_C_MIN) * (double) rand() / RAND_MAX;
    float c23 = RANDOM_C_MIN + (RANDOM_C_MAX - RANDOM_C_MIN) * (double) rand() / RAND_MAX;
    float c33 = RANDOM_C_MIN + (RANDOM_C_MAX - RANDOM_C_MIN) * (double) rand() / RAND_MAX;
    float c44 = RANDOM_C_MIN + (RANDOM_C_MAX - RANDOM_C_MIN) * (double) rand() / RAND_MAX;
    float c55 = RANDOM_C_MIN + (RANDOM_C_MAX - RANDOM_C_MIN) * (double) rand() / RAND_MAX;
    float c66 = RANDOM_C_MIN + (RANDOM_C_MAX - RANDOM_C_MIN) * (double) rand() / RAND_MAX;

    Material::RheologyProperties C;
    C.c11 = c11;
    C.c12 = c12;
    C.c13 = c13;
    C.c14 = 0;
    C.c15 = 0;
    C.c16 = 0;
    C.c22 = c22;
    C.c23 = c23;
    C.c24 = 0;
    C.c25 = 0;
    C.c26 = 0;
    C.c33 = c33;
    C.c34 = 0;
    C.c35 = 0;
    C.c36 = 0;
    C.c44 = c44;
    C.c45 = 0;
    C.c46 = 0;
    C.c55 = c55;
    C.c56 = 0;
    C.c66 = c66;

    float rho = RHO_MIN + (RHO_MAX - RHO_MIN) * (double) rand() / RAND_MAX;
    gcm::real crackThreshold = numeric_limits<gcm::real>::infinity();

    return makeMaterialPtr(name, rho, crackThreshold, C);
};

template<class DecomposerImplementation>
void testDecomposition(MaterialGenerator generator)
{
    srand(0);
    for (int count = 0; count < ITERATIONS; count++) {
        CalcNode anisotropicNode;

        auto mat = generator("test");
        auto matrix = makeRheologyMatrixPtr<AnisotropicRheologyMatrixSetter, DecomposerImplementation>(mat);

        for (int i = 0; i < 3; i++) {
            switch (i) {
            case 0: matrix->decomposeX(anisotropicNode);
                break;
            case 1: matrix->decomposeY(anisotropicNode);
                break;
            case 2: matrix->decomposeZ(anisotropicNode);
                break;
            }

            // Test decomposition
            bool decompositionCorrect =
                    (matrix->getU1() * matrix->getL() * matrix->getU() == matrix->getA());

            if (!decompositionCorrect) {
                LOG_DEBUG(matrix->getU1() * matrix->getL() * matrix->getU());
                LOG_DEBUG(matrix->getA());
            }
            ASSERT_TRUE(decompositionCorrect);

            // Test eigenvalues and eigenvectors
            bool eigenvectorsCorrect =
                    (matrix->getU1() * matrix->getL() == matrix->getA() * matrix->getU1());

            if (!eigenvectorsCorrect) {
                LOG_DEBUG(matrix->getA());
                LOG_DEBUG(matrix->getL());
                LOG_DEBUG(matrix->getU1());
                LOG_DEBUG(matrix->getU());
            }
            ASSERT_TRUE(eigenvectorsCorrect);

        }
        Engine::getInstance().clear();
    }
};

template<class DecomposerImplementation>
void testIsotropicTransition()
{
    srand(0);
    for (int count = 0; count < ITERATIONS; count++) {
        gcm::real la = ISOTROPIC_LAMBDA_MIN + (ISOTROPIC_LAMBDA_MAX - ISOTROPIC_LAMBDA_MIN) * (double) rand() / RAND_MAX;
        gcm::real mu = ISOTROPIC_MU_MIN + (ISOTROPIC_MU_MAX - ISOTROPIC_MU_MIN) * (double) rand() / RAND_MAX;
        gcm::real rho = RHO_MIN + (RHO_MAX - RHO_MIN) * (double) rand() / RAND_MAX;
        gcm::real crackThreshold = numeric_limits<gcm::real>::infinity();

        CalcNode node;

        auto m = makeMaterialPtr("AnisotropicMatrix3D_IsotropicTransition", rho, crackThreshold, la, mu);

        auto isotropicMatrix = makeRheologyMatrixPtr<IsotropicRheologyMatrixSetter, IsotropicRheologyMatrixDecomposer>(m);
        auto anisotropicMatrix = makeRheologyMatrixPtr<AnisotropicRheologyMatrixSetter, DecomposerImplementation>(m);
        
        for (int i = 0; i < 3; i++) {
            switch (i) {
            case 0: isotropicMatrix->decomposeX(node);
                anisotropicMatrix->decomposeX(node);
                break;
            case 1: isotropicMatrix->decomposeY(node);
                anisotropicMatrix->decomposeY(node);
                break;
            case 2: isotropicMatrix->decomposeZ(node);
                anisotropicMatrix->decomposeZ(node);
                break;
            }

            for (int j = 0; j < 9; j++)
                for (int k = 0; k < 9; k++)
                    ASSERT_NEAR(anisotropicMatrix->getA(j, k), isotropicMatrix->getA(j, k), EQUALITY_TOLERANCE);
        }
        Engine::getInstance().clear();
    }
};

void compareInitialMatrices(MaterialGenerator generator)
{
    srand(0);
    for (int count = 0; count < ITERATIONS; count++) {
        CalcNode anisotropicNode;        
        auto mat = generator("test");

        auto analyticalMatrix = makeRheologyMatrixPtr<AnisotropicRheologyMatrixSetter, AnalyticalRheologyMatrixDecomposer>(mat);
        auto numericalMatrix = makeRheologyMatrixPtr<AnisotropicRheologyMatrixSetter, NumericalRheologyMatrixDecomposer>(mat);

        for (int i = 0; i < 3; i++) {
            switch (i) {
            case 0: analyticalMatrix->decomposeX(anisotropicNode);
                numericalMatrix->decomposeX(anisotropicNode);
                break;
            case 1: analyticalMatrix->decomposeY(anisotropicNode);
                numericalMatrix->decomposeY(anisotropicNode);
                break;
            case 2: analyticalMatrix->decomposeZ(anisotropicNode);
                numericalMatrix->decomposeZ(anisotropicNode);
                break;
            }
            ASSERT_TRUE(analyticalMatrix->getA() == numericalMatrix->getA());
        }
        Engine::getInstance().clear();
    }
};

template<class DecomposerImplementation1, class DecomposerImplementation2>
void compareEigenvalues(MaterialGenerator generator)
{
    srand(0);
    for (int count = 0; count < ITERATIONS; count++) {
        CalcNode anisotropicNode;
        auto mat = generator("test");

        auto matrix1 = makeRheologyMatrixPtr<AnisotropicRheologyMatrixSetter, DecomposerImplementation1>(mat);
        auto matrix2 = makeRheologyMatrixPtr<AnisotropicRheologyMatrixSetter, DecomposerImplementation2>(mat);

        for (int i = 0; i < 3; i++) {
            switch (i) {
            case 0: matrix1->decomposeX(anisotropicNode);
                matrix2->decomposeX(anisotropicNode);
                break;
            case 1: matrix1->decomposeY(anisotropicNode);
                matrix2->decomposeY(anisotropicNode);
                break;
            case 2: matrix1->decomposeZ(anisotropicNode);
                matrix2->decomposeZ(anisotropicNode);
                break;
            }

            gcm::real values1[9];
            gcm::real values2[9];
            for (int i = 0; i < 9; i++) {
                values1[i] = matrix1->getL().get(i, i);
                values2[i] = matrix2->getL().get(i, i);
            }
            std::sort(std::begin(values1), std::end(values1));
            std::sort(std::begin(values2), std::end(values2));
            
            for (int i = 0; i < 9; i++) {
                bool equal = 
                    ( (fabs(values1[i]) < EQUALITY_TOLERANCE && fabs(values2[i]) < EQUALITY_TOLERANCE )
                            || fabs(values1[i] - values2[i]) < 0.005 * fabs(values1[i] + values2[i]));
                ASSERT_TRUE( equal );
            }
        }
        Engine::getInstance().clear();
    }
};

void testRotation(int f1, int f2, int f3)
{
    srand(0);
    for (int count = 0; count < ITERATIONS; count++) {
        double a = 2 * M_PI;
        
        for (int i = 1; i <= MAX_ROTATIONS_NUMBER; i++) {
            auto mat = generateRandomMaterial("testRotationMaterial");
            auto p = mat->getRheologyProperties();
            auto p2 = mat->getRheologyProperties();

            for (int k = 1; k <= i; k++)
                p2.rotate(f1 * a / i, f2 * a / i, f3 * a / i);

            for (int j = 0; j < Material::RHEOLOGY_PROPERTIES_NUMBER; j++)
                ASSERT_NEAR(p2.values[j], p.values[j], fabs(p.values[j] + p2.values[j]) * 0.005);
            
            Engine::getInstance().clear();
        }
    }
};

/*
 * Tests implementations
 */

TEST(AnisotropicMatrix3D, AnalyticalFuzzRandom)
{
    testDecomposition<AnalyticalRheologyMatrixDecomposer>(generateRandomMaterial);
};

TEST(AnisotropicMatrix3D, NumericalFuzzRandom)
{
    testDecomposition<NumericalRheologyMatrixDecomposer>(generateRandomMaterial);
};

TEST(AnisotropicMatrix3D, AnalyticalFuzzOrthotropic)
{
    testDecomposition<AnalyticalRheologyMatrixDecomposer>(generateOrthotropicMaterial);
};

TEST(AnisotropicMatrix3D, NumericalFuzzOrthotropic)
{
    testDecomposition<NumericalRheologyMatrixDecomposer>(generateOrthotropicMaterial);
};

TEST(AnisotropicMatrix3D, AnalyticalIsotropicTransition)
{
    testIsotropicTransition<AnalyticalRheologyMatrixDecomposer>();
};

TEST(AnisotropicMatrix3D, NumericalIsotropicTransition)
{
    testIsotropicTransition<NumericalRheologyMatrixDecomposer>();
};

TEST(AnisotropicMatrix3D, InitialMatricesEqualRandom)
{
    compareInitialMatrices(generateRandomMaterial);
};

TEST(AnisotropicMatrix3D, InitialMatricesEqualOrthotropic)
{
    compareInitialMatrices(generateOrthotropicMaterial);
};

TEST(AnisotropicMatrix3D, EigenvaluesEqualRandom)
{
    compareEigenvalues<AnalyticalRheologyMatrixDecomposer, NumericalRheologyMatrixDecomposer>(generateRandomMaterial);
};

TEST(AnisotropicMatrix3D, EigenvaluesEqualOrthotropic)
{
    compareEigenvalues<AnalyticalRheologyMatrixDecomposer, NumericalRheologyMatrixDecomposer>(generateOrthotropicMaterial);
};

TEST(AnisotropicMatrix3D, rotateA1)
{
    testRotation(1, 0, 0);
};

TEST(AnisotropicMatrix3D, rotateA2)
{
    testRotation(0, 1, 0);
};

TEST(AnisotropicMatrix3D, rotateA3)
{
    testRotation(0, 0, 1);
};
