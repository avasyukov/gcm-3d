#include <time.h>
#include <gtest/gtest.h>

#include "materials/IAnisotropicElasticMaterial.h"
#include "materials/AnisotropicElasticMaterial.h"
#include "materials/IsotropicElasticMaterial.h"
#include "util/AnisotropicMatrix3DAnalytical.h"
#include "util/AnisotropicMatrix3D.h"
#include "util/ElasticMatrix3D.h"
#include "Math.h"
#include "Exception.h"

#define ITERATIONS 1000

// Use these limits if anisotropic rheology parameters tensor should be
// isotropic one plus smaller random values
#define LAMBDA_LIMIT 0.0
#define MU_LIMIT 0.0
// Random part of anisotropic rheology parameters tensor
#define RANDOM_LIMIT 1.0e+3

#define RHO_LIMIT 1.0

// Limits for isotropic transition test
#define ISOTROPIC_LAMBDA_LIMIT 1
#define ISOTROPIC_MU_LIMIT 1.0e-1
#define ISOTROPIC_RHO_LIMIT 1.0


class AnisotropicElasticMaterialAnalytical : public Material, public IAnisotropicElasticMaterial
{
protected:
    RheologyParameters rheologyParameters;
    AnisotropicMatrix3DAnalytical matrix;

public:
    AnisotropicElasticMaterialAnalytical(string name, gcm_real rho, gcm_real crackThreshold, RheologyParameters params):  Material(name, rho, crackThreshold), rheologyParameters(params)
    {
    }

    ~AnisotropicElasticMaterialAnalytical()
    {
    }

    const RheologyParameters& getParameters() const
    {
        return rheologyParameters;
    }

    AnisotropicMatrix3DAnalytical& getRheologyMatrix() override
    {
        return matrix;
    }
};


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
	float max = 0;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            matC[i][j] = 0;
            for (int k = 0; k < 6; k++)
                matC[i][j] += L[k][i] * L[k][j];
			max = fmax( max, matC[i][j] );
		}
    }
	
	for (int i = 0; i < 6; i++)
        for (int j = 0; j < 6; j++)
			matC[i][j] = matC[i][j]/max;

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

template<class AnisotropicMatrixImplementation>
void testDecomposition(AnisotropicElasticMaterial(*generateMaterial)(string))
{
    for (int count = 0; count < ITERATIONS; count++) {
        CalcNode anisotropicNode;
        AnisotropicMatrixImplementation matrix;

        string testMaterialName = "AnisotropicMatrix3D_FuzzyMultiplication_" + to_string(count);
        AnisotropicElasticMaterial mat = generateMaterial(testMaterialName);
        anisotropicNode.setMaterialId(Engine::getInstance().addMaterial(&mat));

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
            ASSERT_TRUE( matrix.getU1() * matrix.getL() * matrix.getU() |= matrix.getA() );
            // Test eigen values and eigen rows
            ASSERT_TRUE( matrix.getU1() * matrix.getL() |= matrix.getA() * matrix.getU1() );
        }
        Engine::getInstance().clear();
    }
};

// To build difference between U1 in alalytical and numerical matrixes
void build_U1_Difference(RheologyMatrix3D& analyticalMatrix, RheologyMatrix3D& numericalMatrix) {
    gcm_matrix diff;
    diff.clear();

    int j_an, j_num, k, k_max;
    float eigenvA, max, ratio;

    // Through all eigenvalues
    for(j_an = 0; j_an < 6; j_an++) {
        eigenvA = analyticalMatrix.getL().get(j_an, j_an);

        // Finding the same eigenvalue in numericalMatrix
        j_num = 0;
        while(fabs(eigenvA - numericalMatrix.getL().get(j_num, j_num)) > fmax(fabs(eigenvA), fabs(numericalMatrix.getL().get(j_num, j_num)))*EQUALITY_TOLERANCE) { j_num++; }

        // Finding the maximum component
        max = 0.0;
        k_max = 0;
        for(k = 0; k < 9; k++)
            if(max < fabs(analyticalMatrix.getU1().get(k, j_an))) {
                max = fabs(analyticalMatrix.getU1().get(k, j_an));
                k_max = k;
            }
        ratio = analyticalMatrix.getU1().get(k_max, j_an)/numericalMatrix.getU1().get(k_max, j_num);
        // Build the difference between the same eigenvectors
        for(k = 0; k < 9; k++)
            diff(k, j_an) = fabs(analyticalMatrix.getU1().get(k, j_an) - ratio*numericalMatrix.getU1().get(k, j_num));
    }

    cout << '\n' << analyticalMatrix.getU1() << '\n';
    cout << '\n' << diff << '\n';

};

template<class AnisotropicMatrixImplementation1, class AnisotropicMatrixImplementation2>
void compareDecomposition(AnisotropicElasticMaterial(*generateMaterial)(string))
{
    for (int count = 0; count < ITERATIONS; count++) {
        CalcNode anisotropicNode;
        AnisotropicMatrixImplementation1 matrix1;
        AnisotropicMatrixImplementation2 matrix2;

        string testMaterialName = "AnisotropicMatrix3D_Comparing_" + to_string(count);
        AnisotropicElasticMaterial mat = generateMaterial(testMaterialName);
        anisotropicNode.setMaterialId(Engine::getInstance().addMaterial(&mat));

        for (int i = 0; i < 3; i++) {
            switch (i) {
            case 0: matrix1.createAx(anisotropicNode);
                matrix2.createAx(anisotropicNode);
                break;
            case 1: matrix1.createAy(anisotropicNode);
                matrix2.createAy(anisotropicNode);
                break;
            case 2: matrix1.createAz(anisotropicNode);
                matrix2.createAz(anisotropicNode);
                break;
            }

            int j_an, j_num, k;
            float eigenvA, ratio;

            // Through all eigenvalues
            for(j_an = 0; j_an < 6; j_an++) {
                eigenvA = matrix1.getL().get(j_an, j_an);

                // Finding the same eigenvalue in numericalMatrix
                j_num = 0;
                while(fabs(eigenvA - matrix2.getL().get(j_num, j_num)) > fmax(fabs(eigenvA), fabs(matrix2.getL().get(j_num, j_num)))*10.0*EQUALITY_TOLERANCE) { j_num++; }

                // Finding the first exapmle ratio of components
                k = -1;
                do {
                    k++;
                    ratio = matrix1.getU1().get(k, j_an)/matrix2.getU1().get(k, j_num);
                } while(fabs(matrix2.getU1().get(k, j_num)) < 1.0e-8);

                // Comparing this ratio with another ratios
                for(k = 0; k < 9; k++) {
                    if(fabs(matrix1.getU1().get(k, j_an)) < 1.0e-8) ASSERT_NEAR(matrix2.getU1().get(k, j_num), 0.0, 1.0e-8);
                    else ASSERT_NEAR(ratio, matrix1.getU1().get(k, j_an)/matrix2.getU1().get(k, j_num), fmax(fabs(ratio), fabs(matrix1.getU1().get(k, j_an)/matrix2.getU1().get(k, j_num)))*10.0*EQUALITY_TOLERANCE);
                }
            }
        }
        Engine::getInstance().clear();
    }
};

template<class MaterialImplementation>
void testIsotropicTransition()
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
        MaterialImplementation m2("AnisotropicMatrix3D_IsotropicTransition_AEM", rho, crackThreshold, params);

        isotropicNode.setMaterialId(Engine::getInstance().addMaterial(&m1));
        anisotropicNode.setMaterialId(Engine::getInstance().addMaterial(&m2));

        RheologyMatrix3D& isotropicMatrix = isotropicNode.getRheologyMatrix();
        RheologyMatrix3D& anisotropicMatrix = anisotropicNode.getRheologyMatrix();

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
    testDecomposition<AnisotropicMatrix3DAnalytical>(generateRandomMaterial);
};

TEST(AnisotropicMatrix3D, NumericalFuzzRandom)
{
    srand(time(NULL));
    testDecomposition<AnisotropicMatrix3D>(generateRandomMaterial);
};

TEST(AnisotropicMatrix3D, AnalyticalFuzzOrthotropic)
{
    srand(time(NULL));
    testDecomposition<AnisotropicMatrix3DAnalytical>(generateOrthotropicMaterial);
};

TEST(AnisotropicMatrix3D, NumericalFuzzOrthotropic)
{
    srand(time(NULL));
    testDecomposition<AnisotropicMatrix3D>(generateOrthotropicMaterial);
};

TEST(AnisotropicMatrix3D, AnalyticalIsotropicTransition)
{
    srand(time(NULL));
    testIsotropicTransition<AnisotropicElasticMaterialAnalytical>();
};

TEST(AnisotropicMatrix3D, NumericalIsotropicTransition)
{
    srand(time(NULL));
    testIsotropicTransition<AnisotropicElasticMaterial>();
};

//TEST(AnisotropicMatrix3D, AnalyticalVSNumericalRandom) 
//{
//    srand(time(NULL));
//    compareDecomposition<AnisotropicMatrix3DAnalytical, AnisotropicMatrix3D>(generateRandomMaterial);
//};

TEST(AnisotropicMatrix3D, AnalyticalEqNumerical)
{
    srand(time(NULL));
    for (int count = 0; count < ITERATIONS; count++) {
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

        Engine::getInstance().clear();
    }
};
//
//TEST(AnisotropicMatrix3D, AnalyticalVsNumericalPerf)
//{
//    float MINIMAL_EXPECTED_SPEEDUP = 2.0;
//
//    struct timespec start;
//    struct timespec end;
//    long analyticalTime;
//    long numericalTime;
//
//    AnisotropicMatrix3DAnalytical analyticalMatrices[ITERATIONS];
//    AnisotropicMatrix3D numericalMatrices[ITERATIONS];
//    CalcNode anisotropicNode;
//
//    string testMaterialName = "AnisotropicMatrix3D_Perf";
//    AnisotropicElasticMaterial mat = generateRandomMaterial(testMaterialName);
//    anisotropicNode.setMaterialId(Engine::getInstance().addMaterial(&mat));
//
//    clock_gettime(CLOCK_REALTIME, &start);
//    for (int count = 0; count < ITERATIONS; count++) {
//        analyticalMatrices[count].createAx(anisotropicNode);
//        analyticalMatrices[count].createAy(anisotropicNode);
//        analyticalMatrices[count].createAz(anisotropicNode);
//    }
//    clock_gettime(CLOCK_REALTIME, &end);
//    analyticalTime = (end.tv_sec - start.tv_sec) * 1.0e3 + (end.tv_nsec - start.tv_nsec) / 1.0e6;
//
//    clock_gettime(CLOCK_REALTIME, &start);
//    for (int count = 0; count < ITERATIONS; count++) {
//        numericalMatrices[count].createAx(anisotropicNode);
//        numericalMatrices[count].createAy(anisotropicNode);
//        numericalMatrices[count].createAz(anisotropicNode);
//    }
//    clock_gettime(CLOCK_REALTIME, &end);
//    numericalTime = (end.tv_sec - start.tv_sec) * 1.0e3 + (end.tv_nsec - start.tv_nsec) / 1.0e6;
//
//    ASSERT_GE(numericalTime,  analyticalTime*MINIMAL_EXPECTED_SPEEDUP);
//};
