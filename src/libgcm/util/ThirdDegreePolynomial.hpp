#ifndef THIRDDEGREEPOLYNOMIAL_H
#define    THIRDDEGREEPOLYNOMIAL_H 1

#include "libgcm/util/AnisotropicMatrix3DAnalytical.hpp"
#include "libgcm/materials/IAnisotropicElasticMaterial.hpp"

namespace gcm {
    class ThirdDegreePolynomial
    {
        /*
         * P(x) = x^3 + a*x^2 + b*x + c
         * All the roots are real, different
         * (or two ones are equal),
         * " > 0 " for all the roots
         */
        double a, b, c;
        double roots[3];
        void findRoots();
        bool isMltpl;
    public:
        ThirdDegreePolynomial(float rho, const IAnisotropicElasticMaterial::RheologyParameters &C, int stage);
        ~ThirdDegreePolynomial();
        void getRoots(double *place);
        bool isMultiple();
    };
}

#endif    /* THIRDDEGREEPOLYNOMIAL_H */

