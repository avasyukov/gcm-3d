#include "util/ThirdDegreePolynomial.h"

gcm::ThirdDegreePolynomial::ThirdDegreePolynomial(float rho, 
								const IAnisotropicElasticMaterial::RheologyParameters &C, int stage)
{
	if (stage == 0) {
		a = -1.0/rho * ( C.c11 + C.c55 + C.c66 ) ;
		b = 1.0/rho/rho * ( C.c11*C.c55 + C.c11*C.c66 + C.c66*C.c55 - 
				C.c15*C.c15 - C.c16*C.c16 - C.c56*C.c56 ) ;
		c = 1.0/rho/rho/rho * ( C.c11*C.c56*C.c56 + C.c55*C.c16*C.c16 +
				C.c66*C.c15*C.c15 - C.c11*C.c55*C.c66 - 2*C.c15*C.c16*C.c56 ) ;
	} else if (stage == 1) {
		a = -1.0/rho * ( C.c22 + C.c44 + C.c66 ) ;
		b = 1.0/rho/rho * ( C.c22*C.c44 + C.c22*C.c66 + C.c66*C.c44 - 
				C.c24*C.c24 - C.c26*C.c26 - C.c46*C.c46 ) ;
		c = 1.0/rho/rho/rho * ( C.c22*C.c46*C.c46 + C.c44*C.c26*C.c26 +
				C.c66*C.c24*C.c24 - C.c22*C.c44*C.c66 - 2*C.c24*C.c26*C.c46 ) ;
	} else if (stage == 2) {
		a = -1.0/rho * ( C.c33 + C.c55 + C.c44 ) ;
		b = 1.0/rho/rho * ( C.c33*C.c55 + C.c33*C.c44 + C.c44*C.c55 - 
				C.c34*C.c34 - C.c35*C.c35 - C.c45*C.c45 ) ;
		c = 1.0/rho/rho/rho * ( C.c33*C.c45*C.c45 + C.c44*C.c35*C.c35 +
				C.c55*C.c34*C.c34 - C.c33*C.c55*C.c44 - 2*C.c35*C.c34*C.c45 ) ;
	} else {
		THROW_BAD_CONFIG("Wrong stage number");
	}
	findRoots();
};

void gcm::ThirdDegreePolynomial::findRoots()
{
	isMultiple = false;
	double Q = (a*a - 3*b)/9;
	double R = (2*a*a*a - 9*a*b + 27*c)/54;
	if ( abs (R / sqrt(Q*Q*Q)) > 1 - EQUALITY_TOLERANCE ) {
		// Two roots are equal
		isMultiple = true;
		if (R > 0) {
			roots[0] = - sqrt(Q) - a/3;
			roots[1] = roots[2] = 2 * sqrt(Q) - a/3;
		} else {
			roots[0] = 2 * sqrt(Q) - a/3;
			roots[1] = roots[2] = - sqrt(Q) - a/3;
		}
	} else {
		// All the roots are different 
		double phi = acos (R / sqrt(Q*Q*Q)) / 3;
		roots[0] = -2 * sqrt(Q) * cos (phi) - a/3;
		roots[1] = -2 * sqrt(Q) * cos (phi + 2 * M_PI / 3) - a/3;
		roots[2] = -2 * sqrt(Q) * cos (phi - 2 * M_PI / 3) - a/3;
		// Checking roots
		for (int i = 0; i < 3; i++) {
			double xR = roots[i] * (1 + 1e+3 * EQUALITY_TOLERANCE);
			double xL = roots[i] * (1 - 1e+3 * EQUALITY_TOLERANCE);
			if( ((xR * xR * xR + a * xR * xR + b * xR + c) * 
					(xL * xL * xL + a * xL * xL + b * xL + c) > 0)
				|| (roots[i] < 0) )
			{
				THROW_INVALID_INPUT("Bad roots!");
			}
		}
	}

};

gcm::ThirdDegreePolynomial::~ThirdDegreePolynomial() {};
		
void gcm::ThirdDegreePolynomial::getRoots(float *place)
{
	for (int i = 0; i < 3; i++){
		place[i] = roots[i];
	}
};

