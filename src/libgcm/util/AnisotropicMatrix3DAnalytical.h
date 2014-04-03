#ifndef ANISOTROPICMATRIX3DANALYTICAL_H
#define ANISOTROPICMATRIX3DANALYTICAL_H  1

#include <assert.h>

#include "materials/IAnisotropicElasticMaterial.h"
#include "util/MutableRheologyMatrix3D.h"
#include "util/matrixes.h"
#include "util/MatrixInverter.h"
#include "util/ThirdDegreePolynomial.h"
#include "Exception.h"
#include <iomanip>

using namespace gcm;

namespace gcm {

    /**
     * Anisotropic rheology matrix implementation.
     * Creates corresponding rheology matrices for case of anisotropic material.
     * Implements 'semi-analytical' solution.
     */
	class AnisotropicMatrix3DAnalytical : public MutableRheologyMatrix3D
	{
	public:
		void createAx(const ICalcNode& node) override;
		void createAy(const ICalcNode& node) override;
		void createAz(const ICalcNode& node) override;
	private:
		
		void clear();
		void fixValuesOrder();
		void findNonZeroSolution(double **M, double *x);
		void findNonZeroSolution(double **M, double *x, double *y);
		void findEigenVec(double *eigenVec,
					double l, float rho, const IAnisotropicElasticMaterial::RheologyParameters &C, int stage);
		void findEigenVec(double *eigenVec1, double *eigenVec2,
					double l, float rho, const IAnisotropicElasticMaterial::RheologyParameters &C, int stage);
	};
}

#endif

