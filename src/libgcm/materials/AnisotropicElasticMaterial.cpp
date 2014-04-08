#include "libgcm/materials/AnisotropicElasticMaterial.hpp"
#include "libgcm/Exception.hpp"

#include <cassert>

gcm::AnisotropicElasticMaterial::AnisotropicElasticMaterial(string name, gcm_real rho, gcm_real crackThreshold, 
                RheologyParameters params) 
        : Material(name, rho, crackThreshold), rheologyParameters(params)
{
    matrix = new AnisotropicMatrix3D();
}

gcm::AnisotropicElasticMaterial::AnisotropicElasticMaterial(string name, gcm_real rho, gcm_real crackThreshold, 
                RheologyParameters params, RheologyMatrix3D* anisotropicMatrixImplementation) 
        : Material(name, rho, crackThreshold), rheologyParameters(params)
{
    matrix = anisotropicMatrixImplementation;
}

gcm::AnisotropicElasticMaterial::~AnisotropicElasticMaterial()
{
    delete matrix;
}

void gcm::AnisotropicElasticMaterial::rotate(float a1, float a2, float a3)
{
	/**
	 *  
	 * Change RheologyParameters
	 * by angles of rotation
	 * 
	 * Our system of axes (where we do calculations)
	 * is rotated relative to the system,
	 * where initial RheologyParameters are set,
	 * firstly, by a1 radians around x-axis clockwise,
	 * secondly, by a2 radians around y-axis clockwise,
	 * thirdly, by a3 radians around z-axis clockwise
	 * 
	 */
	
	float G[3][3];	// matrix of rotation
	G[0][0] = cos(a2)*cos(a3);
	G[0][1] = cos(a3)*sin(a1)*sin(a2) - sin(a3)*cos(a1);
	G[0][2] = cos(a1)*sin(a2)*cos(a3) + sin(a1)*sin(a3);
	G[1][0] = cos(a2)*sin(a3);
	G[1][1] = sin(a3)*sin(a1)*sin(a2) + cos(a3)*cos(a1);
	G[1][2] = cos(a1)*sin(a2)*sin(a3) - sin(a1)*cos(a3);
	G[2][0] = -sin(a2);
	G[2][1] = sin(a1)*cos(a2);
	G[2][2] = cos(a1)*cos(a2);
	
	float C[3][3][3][3];	// old tensor

	C[0][0][0][0] = rheologyParameters.c11; 
	C[1][1][1][1] = rheologyParameters.c22; 
	C[2][2][2][2] = rheologyParameters.c33; 

	C[0][0][1][1] = C[1][1][0][0] = rheologyParameters.c12; 
	C[0][0][2][2] = C[2][2][0][0] = rheologyParameters.c13; 
	C[1][1][2][2] = C[2][2][1][1] = rheologyParameters.c23; 

	C[0][0][1][2] = C[0][0][2][1] = C[1][2][0][0] = C[2][1][0][0] = rheologyParameters.c14; 
	C[0][0][0][2] = C[0][0][2][0] = C[0][2][0][0] = C[2][0][0][0] = rheologyParameters.c15; 
	C[0][0][0][1] = C[0][0][1][0] = C[0][1][0][0] = C[1][0][0][0] = rheologyParameters.c16; 

	C[1][1][1][2] = C[1][1][2][1] = C[1][2][1][1] = C[2][1][1][1] = rheologyParameters.c24; 
	C[1][1][0][2] = C[1][1][2][0] = C[0][2][1][1] = C[2][0][1][1] = rheologyParameters.c25; 
	C[1][1][0][1] = C[1][1][1][0] = C[0][1][1][1] = C[1][0][1][1] = rheologyParameters.c26; 

	C[2][2][1][2] = C[2][2][2][1] = C[1][2][2][2] = C[2][1][2][2] = rheologyParameters.c34; 
	C[2][2][0][2] = C[2][2][2][0] = C[0][2][2][2] = C[2][0][2][2] = rheologyParameters.c35; 
	C[2][2][0][1] = C[2][2][1][0] = C[0][1][2][2] = C[1][0][2][2] = rheologyParameters.c36; 

	C[0][2][0][2] = C[2][0][0][2] = C[0][2][2][0] = C[2][0][2][0] = rheologyParameters.c55; 
	C[1][2][1][2] = C[2][1][1][2] = C[1][2][2][1] = C[2][1][2][1] = rheologyParameters.c44; 
	C[0][1][0][1] = C[1][0][0][1] = C[0][1][1][0] = C[1][0][1][0] = rheologyParameters.c66; 

	C[1][2][0][2] = C[2][1][0][2] = C[1][2][2][0] = C[2][1][2][0] =
	C[0][2][1][2] = C[0][2][2][1] = C[2][0][1][2] = C[2][0][2][1] = rheologyParameters.c45; 

	C[1][2][0][1] = C[2][1][0][1] = C[1][2][1][0] = C[2][1][1][0] =
	C[0][1][1][2] = C[0][1][2][1] = C[1][0][1][2] = C[1][0][2][1] = rheologyParameters.c46; 

	C[0][2][0][1] = C[2][0][0][1] = C[0][2][1][0] = C[2][0][1][0] =
	C[0][1][0][2] = C[0][1][2][0] = C[1][0][0][2] = C[1][0][2][0] = rheologyParameters.c56; 

	float C1[3][3][3][3];	// new tensor
	
	for (int m = 0; m < 3; m++)	// rotation 
	for (int n = 0; n < 3; n++) 
	for (int p = 0; p < 3; p++) 
	for (int q = 0; q < 3; q++) {
		C1[m][n][p][q] = 0;
		for (int i = 0; i < 3; i++) 
		for (int j = 0; j < 3; j++) 
		for (int k = 0; k < 3; k++) 
		for (int l = 0; l < 3; l++)
			C1[m][n][p][q] += G[m][i]*G[n][j]*G[p][k]*G[q][l]*C[i][j][k][l];
	}

	rheologyParameters.c11 = C1[0][0][0][0]; 
	rheologyParameters.c22 = C1[1][1][1][1]; 
	rheologyParameters.c33 = C1[2][2][2][2]; 
	rheologyParameters.c12 = C1[0][0][1][1]; 
	rheologyParameters.c13 = C1[0][0][2][2]; 
	rheologyParameters.c23 = C1[1][1][2][2]; 
	rheologyParameters.c14 = C1[0][0][1][2]; 
	rheologyParameters.c15 = C1[0][0][0][2]; 
	rheologyParameters.c16 = C1[0][0][0][1]; 
	rheologyParameters.c24 = C1[1][1][1][2]; 
	rheologyParameters.c25 = C1[1][1][0][2]; 
	rheologyParameters.c26 = C1[1][1][0][1]; 
	rheologyParameters.c34 = C1[2][2][1][2]; 
	rheologyParameters.c35 = C1[2][2][0][2]; 
	rheologyParameters.c36 = C1[2][2][0][1]; 
	rheologyParameters.c55 = C1[0][2][0][2]; 
	rheologyParameters.c44 = C1[1][2][1][2]; 
	rheologyParameters.c66 = C1[0][1][0][1]; 
	rheologyParameters.c45 = C1[0][2][1][2]; 
	rheologyParameters.c46 = C1[0][1][1][2]; 
	rheologyParameters.c56 = C1[0][1][0][2]; 
}

const gcm::IAnisotropicElasticMaterial::RheologyParameters& gcm::AnisotropicElasticMaterial::getParameters() const
{
    return rheologyParameters;
}

//void gcm::AnisotropicElasticMaterial::prepareRheologyMatrixX(const CalcNode& node)
//{
////    matrix.prepareMatrix({}, stage);
//}
//
//void gcm::AnisotropicElasticMaterial::prepareRheologyMatrixY(const CalcNode& node)
//{
////    matrix.prepareMatrix({}, stage);
//}
//
//void gcm::AnisotropicElasticMaterial::prepareRheologyMatrixZ(const CalcNode& node)
//{
////    matrix.prepareMatrix({}, stage);
//}

RheologyMatrix3D& gcm::AnisotropicElasticMaterial::getRheologyMatrix()
{
    return (*matrix);
}
