#ifndef ANISOTROPICMATRIX3DANALYTICAL_H
#define ANISOTROPICMATRIX3DANALYTICAL_H  1

#include <assert.h>

#include "util/matrixes.h"
#include "util/ThirdDegreePolynomial.h"
#include "Exception.h"

using namespace gcm;

namespace gcm {
	
	typedef struct
	{
		union 
		{
			float c[21];
			struct
			{
				float c11, c12, c13, c14, c15, c16;
				float      c22, c23, c24, c25, c26;
				float 		    c33, c34, c35, c36;
				float 				 c44, c45, c46;
				float 					  c55, c56;
				float 						   c66;
			};
		};
	} AnisotropicNumbers;
	
	class AnisotropicMatrix3DAnalytical
	{
	public:
		AnisotropicMatrix3DAnalytical();
		~AnisotropicMatrix3DAnalytical();
		void prepare_matrix(const AnisotropicNumbers &C, float ro, int stage);
		void prepare_matrix(const AnisotropicNumbers &C, float ro, float qjx, float qjy, float qjz);
		float max_lambda();

		void self_check();

		gcm_matrix A;
		gcm_matrix L;
		gcm_matrix U;
		gcm_matrix U1;

	private:
		void CreateAx(const AnisotropicNumbers &C, float ro);
		void CreateAy(const AnisotropicNumbers &C, float ro);
		void CreateAz(const AnisotropicNumbers &C, float ro);
		void CreateGeneralizedMatrix(const AnisotropicNumbers &C, float ro, 
											float qjx, float qjy, float qjz);
		void createMatrixN(int i, int j, float *res);
		void zero_all();
		void findNonZeroSolution(float **M, float *x);
		void findEigenVec(float *eigenVec,
					float l, float rho, const AnisotropicNumbers &C, int stage);

		// TODO it is worth turning them into local vars
		float n[3][3];
	};
}

#endif

