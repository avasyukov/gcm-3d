#include "util/ThirdDegreePolynomial.h"

gcm::ThirdDegreePolynomial::ThirdDegreePolynomial(float rho, 
								const IAnisotropicElasticMaterial::RheologyParameters &C, int stage)
{
	if (stage == 0) {
		a = -1/rho * ( C.c11 + C.c55 + C.c66 ) ;
		b = 1/rho/rho * ( C.c11*C.c55 + C.c11*C.c66 + C.c66*C.c55 - 
				C.c15*C.c15 - C.c16*C.c16 - C.c56*C.c56 ) ;
		c = 1/rho/rho/rho * ( C.c11*C.c56*C.c56 + C.c55*C.c16*C.c16 +
				C.c66*C.c15*C.c15 - C.c11*C.c55*C.c66 - 2*C.c15*C.c16*C.c56 ) ;
	} else if (stage == 1) {
		a = -1/rho * ( C.c22 + C.c44 + C.c66 ) ;
		b = 1/rho/rho * ( C.c22*C.c44 + C.c22*C.c66 + C.c66*C.c44 - 
				C.c24*C.c24 - C.c26*C.c26 - C.c46*C.c46 ) ;
		c = 1/rho/rho/rho * ( C.c22*C.c46*C.c46 + C.c44*C.c26*C.c26 +
				C.c66*C.c24*C.c24 - C.c22*C.c44*C.c66 - 2*C.c24*C.c26*C.c46 ) ;
	} else if (stage == 2) {
		a = -1/rho * ( C.c33 + C.c55 + C.c44 ) ;
		b = 1/rho/rho * ( C.c33*C.c55 + C.c33*C.c44 + C.c44*C.c55 - 
				C.c34*C.c34 - C.c35*C.c35 - C.c45*C.c45 ) ;
		c = 1/rho/rho/rho * ( C.c33*C.c45*C.c45 + C.c44*C.c35*C.c35 +
				C.c55*C.c34*C.c34 - C.c33*C.c55*C.c44 - 2*C.c35*C.c34*C.c45 ) ;
	} else {
		THROW_BAD_CONFIG("Wrong stage number (from ThirdDegreePolynomial)");
	}
	
	findRoots();
};

void gcm::ThirdDegreePolynomial::findRoots()
{
	float Q = (a*a - 3*b)/9;
	float R = (2*a*a*a - 9*a*b + 27*c)/54;
	if (Q*Q*Q - R*R <= 0) {
		THROW_BAD_CONFIG("Therearen't 3 different real roots");
	}
	float phi = 1/3 * acos (R / sqrt(Q*Q*Q));
	roots[0] = -2 * sqrt(Q) * cos (phi) - a/3;
	roots[1] = -2 * sqrt(Q + 2/3 * M_PI) * cos (phi) - a/3;
	roots[2] = -2 * sqrt(Q - 2/3 * M_PI) * cos (phi) - a/3;
	
	for (int i = 0; i < 3; i++) {
		assert( roots[i] > 0 );
	}
};

gcm::ThirdDegreePolynomial::~ThirdDegreePolynomial() {};
		
void gcm::ThirdDegreePolynomial::getRoots(float *place)
{
	for (int i = 0; i < 3; i++){
		place[i] = roots[i];
	}
};

