#ifndef ANISOTROPICMATRIX3DANALYTICAL_H
#define ANISOTROPICMATRIX3DANALYTICAL_H  1

#include <assert.h>

#include "materials/IAnisotropicElasticMaterial.h"
#include "util/matrixes.h"
#include "util/ThirdDegreePolynomial.h"
#include "Exception.h"

using namespace gcm;

namespace gcm {
	
	class AnisotropicMatrix3DAnalytical
	{
	public:
		AnisotropicMatrix3DAnalytical();
		~AnisotropicMatrix3DAnalytical();
		void prepare_matrix(const IAnisotropicElasticMaterial::RheologyParameters &C, float rho, int stage);
		void prepare_matrix(const IAnisotropicElasticMaterial::RheologyParameters &C, float rho, float qjx, float qjy, float qjz);
		float max_lambda();

		void self_check();

		gcm_matrix A;
		gcm_matrix L;
		gcm_matrix U;
		gcm_matrix U1;

	private:
		void CreateAx(const IAnisotropicElasticMaterial::RheologyParameters &C, float rho);
		void CreateAy(const IAnisotropicElasticMaterial::RheologyParameters &C, float rho);
		void CreateAz(const IAnisotropicElasticMaterial::RheologyParameters &C, float rho);
		void CreateGeneralizedMatrix(const IAnisotropicElasticMaterial::RheologyParameters &C, float rho, 
											float qjx, float qjy, float qjz);
		void createMatrixN(int i, int j, float *res);
		void zero_all();
		void findNonZeroSolution(float **M, float *x);
		void findEigenVec(float *eigenVec,
					float l, float rho, const IAnisotropicElasticMaterial::RheologyParameters &C, int stage);

		// TODO it is worth turning them into local vars
		float n[3][3];
	};
}

#endif

