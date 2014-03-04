#include <time.h>
#include <gtest/gtest.h>

#include "util/AnisotropicMatrix3DAnalytical.h"

#define ITERATIONS 100
#define tmpmin 1
#define tmpmax 100


TEST(AnisotropicMatrix3DAnalytical, FuzzyMultiplication) 
{
	srand(time(NULL));
	float L[6][6];
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < i; j++)
			L[i][j] = 0;
		
		L[i][i] = 1.0 * rand()/RAND_MAX * (tmpmax - tmpmin) + tmpmin;
		for (int j = i+1; j < 6; j++)
			L[i][j] = 1.0 * rand()/RAND_MAX * tmpmax;
	}
	
	float matC[6][6];
	for (int i = 0; i < 6; i++)
		for (int j = 0; j < 6; j++) {
			matC[i][j] = 0;
//			for (int k = 0; k < 6; k++)
//				matC[i][j] += L[k][i] * L[k][j];
		}	
	
	gcm::AnisotropicNumbers C;
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

	float rho = 1.0 * rand()/RAND_MAX * (tmpmax - tmpmin) + tmpmin;
	
	
	C.c11 = 1.06e+6;
	C.c12 = 1.8e+5;
	C.c13 = 1.5e+4;
	C.c66 = 4.4e+5;
	C.c55 = 2.0e+2;
	rho = 2.0;	
	
	gcm::AnisotropicMatrix3DAnalytical m;
	for (int stg = 0; stg < 3; stg=stg+1) {
		m.prepare_matrix(C, rho, stg);
		m.self_check();
	}
}
