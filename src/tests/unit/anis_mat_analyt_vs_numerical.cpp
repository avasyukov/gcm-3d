#include <time.h>
#include <gtest/gtest.h>

#include "util/AnisotropicMatrix3DAnalytical.h"
#include "util/AnisotropicMatrix3D.h"
#include "util/ElasticMatrix3D.h"

#define ITERATIONS 100
#define tmpmin 1
#define tmpmax 100


using namespace gcm;

//class AnisotropicAnalyticalTester : public AnisotropicMatrix3DAnalytical{
//	public :
//	void doTest(AnisotropicNumbers &C, gcm_real &rho, int stage) {
//		prepareMatrix(
//		{ C.c[0], C.c[1], C.c[2], C.c[3], C.c[4], C.c[5], C.c[6], C.c[7],
//			C.c[8], C.c[9], C.c[10], C.c[11], C.c[12], C.c[13], C.c[14], C.c[15],
//			C.c[16], C.c[17], C.c[18], C.c[19], C.c[20], rho}, stage);
//	}
//};
//
//class AnisotropicNumericalTester : public AnisotropicMatrix3D{
//	public :
//	void doTest(AnisotropicNumbers &C, gcm_real &rho, int stage) {
//		prepareMatrix(
//		{ C.c[0], C.c[1], C.c[2], C.c[3], C.c[4], C.c[5], C.c[6], C.c[7],
//			C.c[8], C.c[9], C.c[10], C.c[11], C.c[12], C.c[13], C.c[14], C.c[15],
//			C.c[16], C.c[17], C.c[18], C.c[19], C.c[20], rho}, stage);
//	}
//};
//
//TEST(AnisotropicMatrix3DAnalytical, Analyt_vs_Numerical) {
//
//	// Generating random AnisotropicNumbers and rho
//	srand(time(NULL));
//	for (int count = 0; count < ITERATIONS; count++) {
//		gcm_real L[6][6];
//		for (int i = 0; i < 6; i++) {
//			for (int j = 0; j < i; j++)
//				L[i][j] = 0;
//
//			L[i][i] = 1.0 * rand() / RAND_MAX * (tmpmax - tmpmin) + tmpmin;
//			for (int j = i + 1; j < 6; j++)
//				L[i][j] = 1.0 * rand() / RAND_MAX * tmpmax;
//		}
//
//		gcm_real matC[6][6];
//		for (int i = 0; i < 6; i++)
//			for (int j = 0; j < 6; j++) {
//				matC[i][j] = 0;
//				for (int k = 0; k < 6; k++)
//					matC[i][j] += L[k][i] * L[k][j];
//			}
//
//		AnisotropicNumbers C;
//		C.c11 = matC[0][0];
//		C.c12 = matC[0][1];
//		C.c13 = matC[0][2];
//		C.c14 = matC[0][3];
//		C.c15 = matC[0][4];
//		C.c16 = matC[0][5];
//		C.c22 = matC[1][1];
//		C.c23 = matC[1][2];
//		C.c24 = matC[1][3];
//		C.c25 = matC[1][4];
//		C.c26 = matC[1][5];
//		C.c33 = matC[2][2];
//		C.c34 = matC[2][3];
//		C.c35 = matC[2][4];
//		C.c36 = matC[2][5];
//		C.c44 = matC[3][3];
//		C.c45 = matC[3][4];
//		C.c46 = matC[3][5];
//		C.c55 = matC[4][4];
//		C.c56 = matC[4][5];
//		C.c66 = matC[5][5];
//
//		gcm_real rho = 1.0 * rand() / RAND_MAX * (tmpmax - tmpmin) + tmpmin;
//		
//		// Comparing two these classes
//		AnisotropicAnalyticalTester analytic;
//		AnisotropicNumericalTester numeric;
//		
//		for (int stg = 0; stg < 3; stg++) {
//			analytic.doTest(C, rho, stg);
//			cout << endl << "--------------------\n";
//			numeric.doTest(C, rho, stg);
//			
//			for (int j = 0; j < 9; j++)
//				for (int k = 0; k < 9; k++)
//					ASSERT_NEAR(analytic.getA(j, k), numeric.getA(j, k), 1e-6);
//		}
//	}
//}
//
//class AnisotropicMatrixIsotropicWrapper : public AnisotropicMatrix3DAnalytical {
//	public :
//	void prepareIsotropic(gcm_real la, gcm_real mu, gcm_real rho, short int stage) {
//		prepareMatrix(
//		{
//			la + 2 * mu, la, la, 0.0, 0.0, 0.0,
//			la + 2 * mu, la, 0.0, 0.0, 0.0,
//			la + 2 * mu, 0.0, 0.0, 0.0,
//			mu, 0.0, 0.0,
//			mu, 0.0,
//			mu,
//			rho
//		}, stage);
//	}
//};
//
//TEST(AnisotropicMatrix3DAnalytical, IsotropicTransition) {
//	srand(time(NULL));
//
//	for (int cntr = 0; cntr < ITERATIONS; cntr++) {
//		AnisotropicMatrixIsotropicWrapper anisotropicMatrix;
//		ElasticMatrix3D isotropicMatrix;
//
//		gcm_real la = 1.0e+9 * (double) rand() / RAND_MAX;
//		gcm_real mu = 1.0e+8 * (double) rand() / RAND_MAX;
//		gcm_real rho = 1.0e+4 * (double) rand() / RAND_MAX;
//
//		for (int i = 0; i < 3; i++) {
//			isotropicMatrix.prepareMatrix({la, mu, rho}, i);
//			anisotropicMatrix.prepareIsotropic(la, mu, rho, i);
//
//			for (int j = 0; j < 9; j++)
//				for (int k = 0; k < 9; k++)
//					ASSERT_NEAR(anisotropicMatrix.getA(j, k),
//							isotropicMatrix.getA(j, k), 1e-6);
//		}
//	}
//};
//
