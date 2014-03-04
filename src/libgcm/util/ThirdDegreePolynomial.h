#ifndef THIRDDEGREEPOLYNOMIAL_H
#define	THIRDDEGREEPOLYNOMIAL_H 1

#include "util/AnisotropicMatrix3DAnalytical.h"

namespace gcm {
	class ThirdDegreePolynomial
	{
		/*
		 * P(x) = x^3 + a*x^2 + b*x + c
		 * All the roots are different 
		 * or two ones are equal
		 * " >= 0 " for all the roots
		 */
		float a, b, c;
		float roots[3];
		void findRoots();
	public:
		ThirdDegreePolynomial(float rho, const AnisotropicNumbers &C, int stage);
		~ThirdDegreePolynomial();
		void getRoots(float *place);
		bool isMultiple;
	};
}

#endif	/* THIRDDEGREEPOLYNOMIAL_H */

