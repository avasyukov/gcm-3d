#ifndef THIRDDEGREEPOLYNOMIAL_H
#define	THIRDDEGREEPOLYNOMIAL_H 1

#include "util/AnisotropicMatrix3DAnalytical.h"
#include "materials/IAnisotropicElasticMaterial.h"

namespace gcm {
	class ThirdDegreePolynomial
	{
		/*
		 * P(x) = x^3 + a*x^2 + b*x + c
		 * All the roots are different and " > 0 "
		 */
		float a, b, c;
		float roots[3];
		void findRoots();
	public:
		ThirdDegreePolynomial(float rho, const IAnisotropicElasticMaterial::RheologyParameters &C, int stage);
		~ThirdDegreePolynomial();
		void getRoots(float *place);
	};
}

#endif	/* THIRDDEGREEPOLYNOMIAL_H */

