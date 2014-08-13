#include "libgcm/rheology/Material.hpp"

#include <cmath>

using namespace gcm;

Material::RheologyProperties::RheologyProperties()
{
}

Material::RheologyProperties::RheologyProperties(real la, real mu)
{
    c11 = c22 = c33 = la + 2 * mu;
    c44 = c55 = c66 = mu;
    c12 = c13 = c23 = la;
    c14 = c15 = c16 = 0.0;
    c24 = c25 = c26 = 0.0;
    c34 = c35 = c36 = 0.0;
    c45 = c46 = c56 = 0.0;
}

real Material::RheologyProperties::getMu() const
{
    return c66;
}

real Material::RheologyProperties::getLa() const
{
    return c23;
}

void Material::RheologyProperties::rotate(double a1, double a2, double a3)
{
	/**
	 *  
	 * Change RheologyProperties
	 * by angles of rotation
	 * 
	 * Our system of axes (where we do calculations)
	 * is rotated relative to the system,
	 * where initial RheologyProperties are set,
	 * firstly, by a1 radians around x-axis clockwise,
	 * secondly, by a2 radians around y-axis clockwise,
	 * thirdly, by a3 radians around z-axis clockwise
	 * 
	 */
	
	double G[3][3];	// matrix of rotation
	G[0][0] = cos(a2)*cos(a3);
	G[0][1] = cos(a3)*sin(a1)*sin(a2) - sin(a3)*cos(a1);
	G[0][2] = cos(a1)*sin(a2)*cos(a3) + sin(a1)*sin(a3);
	G[1][0] = cos(a2)*sin(a3);
	G[1][1] = sin(a3)*sin(a1)*sin(a2) + cos(a3)*cos(a1);
	G[1][2] = cos(a1)*sin(a2)*sin(a3) - sin(a1)*cos(a3);
	G[2][0] = -sin(a2);
	G[2][1] = sin(a1)*cos(a2);
	G[2][2] = cos(a1)*cos(a2);
	
	double C[3][3][3][3];	// old tensor

	C[0][0][0][0] = c11; 
	C[1][1][1][1] = c22; 
	C[2][2][2][2] = c33; 

	C[0][0][1][1] = C[1][1][0][0] = c12; 
	C[0][0][2][2] = C[2][2][0][0] = c13; 
	C[1][1][2][2] = C[2][2][1][1] = c23; 

	C[0][0][1][2] = C[0][0][2][1] = C[1][2][0][0] = C[2][1][0][0] = c14; 
	C[0][0][0][2] = C[0][0][2][0] = C[0][2][0][0] = C[2][0][0][0] = c15; 
	C[0][0][0][1] = C[0][0][1][0] = C[0][1][0][0] = C[1][0][0][0] = c16; 

	C[1][1][1][2] = C[1][1][2][1] = C[1][2][1][1] = C[2][1][1][1] = c24; 
	C[1][1][0][2] = C[1][1][2][0] = C[0][2][1][1] = C[2][0][1][1] = c25; 
	C[1][1][0][1] = C[1][1][1][0] = C[0][1][1][1] = C[1][0][1][1] = c26; 

	C[2][2][1][2] = C[2][2][2][1] = C[1][2][2][2] = C[2][1][2][2] = c34; 
	C[2][2][0][2] = C[2][2][2][0] = C[0][2][2][2] = C[2][0][2][2] = c35; 
	C[2][2][0][1] = C[2][2][1][0] = C[0][1][2][2] = C[1][0][2][2] = c36; 

	C[0][2][0][2] = C[2][0][0][2] = C[0][2][2][0] = C[2][0][2][0] = c55; 
	C[1][2][1][2] = C[2][1][1][2] = C[1][2][2][1] = C[2][1][2][1] = c44; 
	C[0][1][0][1] = C[1][0][0][1] = C[0][1][1][0] = C[1][0][1][0] = c66; 

	C[1][2][0][2] = C[2][1][0][2] = C[1][2][2][0] = C[2][1][2][0] =
	C[0][2][1][2] = C[0][2][2][1] = C[2][0][1][2] = C[2][0][2][1] = c45; 

	C[1][2][0][1] = C[2][1][0][1] = C[1][2][1][0] = C[2][1][1][0] =
	C[0][1][1][2] = C[0][1][2][1] = C[1][0][1][2] = C[1][0][2][1] = c46; 

	C[0][2][0][1] = C[2][0][0][1] = C[0][2][1][0] = C[2][0][1][0] =
	C[0][1][0][2] = C[0][1][2][0] = C[1][0][0][2] = C[1][0][2][0] = c56; 

	double C1[3][3][3][3];	// new tensor
	
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

	c11 = C1[0][0][0][0]; 
	c22 = C1[1][1][1][1]; 
	c33 = C1[2][2][2][2]; 
	c12 = C1[0][0][1][1]; 
	c13 = C1[0][0][2][2]; 
	c23 = C1[1][1][2][2]; 
	c14 = C1[0][0][1][2]; 
	c15 = C1[0][0][0][2]; 
	c16 = C1[0][0][0][1]; 
	c24 = C1[1][1][1][2]; 
	c25 = C1[1][1][0][2]; 
	c26 = C1[1][1][0][1]; 
	c34 = C1[2][2][1][2]; 
	c35 = C1[2][2][0][2]; 
	c36 = C1[2][2][0][1]; 
	c55 = C1[0][2][0][2]; 
	c44 = C1[1][2][1][2]; 
	c66 = C1[0][1][0][1]; 
	c45 = C1[0][2][1][2]; 
	c46 = C1[0][1][1][2]; 
	c56 = C1[0][1][0][2]; 
}

Material::Material(string name, real rho, real crackThreshold, real la, real mu): Material(name, rho, crackThreshold, la, mu, PlasticityProperties())
{
}

Material::Material(string name, real rho, real crackThreshold, real la, real mu, PlasticityProperties plasticityProps): Material(name, rho, crackThreshold, RheologyProperties(la, mu), plasticityProps)
{
    isotropic = true;
}

Material::Material(string name, real rho, real crackThreshold, RheologyProperties rheologyProps): Material(name, rho, crackThreshold, rheologyProps, PlasticityProperties())
{
}

Material::Material(string name, real rho, real crackThreshold, RheologyProperties rheologyProps, PlasticityProperties plasticityProps): name(name), rho(rho), crackThreshold(crackThreshold), rheologyProps(rheologyProps), plasticityProps(plasticityProps)
{
}
        
Material::Material(const shared_ptr<Material>& source, string name, double a1, double a2, double a3): Material(name, source->rho, source->crackThreshold, source->rheologyProps, source->plasticityProps)
{
    rheologyProps.rotate(a1, a2, a3);
}

const string& Material::getName() const
{
    return name;
}

real Material::getCrackThreshold() const
{
    return crackThreshold;
}

real Material::getRho() const
{
    return rho;
}

bool Material::isIsotropic() const
{
    return isotropic;
}

real Material::getMu() const
{
    return rheologyProps.getMu();
}

real Material::getLa() const
{
    return rheologyProps.getLa();
}

const Material::RheologyProperties& Material::getRheologyProperties() const
{
    return rheologyProps;
}

const Material::PlasticityProperties& Material::getPlasticityProperties() const
{
    return plasticityProps;
}
