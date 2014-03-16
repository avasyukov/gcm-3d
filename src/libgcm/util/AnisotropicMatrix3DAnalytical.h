#ifndef ANISOTROPICMATRIX3DANALYTICAL_H
#define ANISOTROPICMATRIX3DANALYTICAL_H  1

#include <assert.h>

#include "materials/IAnisotropicElasticMaterial.h"
#include "util/RheologyMatrix3D.h"
#include "util/matrixes.h"
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
	class AnisotropicMatrix3DAnalytical : public RheologyMatrix3D
	{
	public:
		void createAx(const ICalcNode& node) override;
		void createAy(const ICalcNode& node) override;
		void createAz(const ICalcNode& node) override;
	private:
		void clear();
		void fixValuesOrder();
		void findNonZeroSolution(float **M, float *x);
		void findNonZeroSolution(float **M, float *x, float *y);
		void findEigenVec(float *eigenVec,
					float l, float rho, const IAnisotropicElasticMaterial::RheologyParameters &C, int stage);
		void findEigenVec(float *eigenVec1, float *eigenVec2,
					float l, float rho, const IAnisotropicElasticMaterial::RheologyParameters &C, int stage);
	};
}

#endif

