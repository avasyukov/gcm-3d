#ifndef _GCM_RHEOLOGY_MATRIX_3D_H
#define _GCM_RHEOLOGY_MATRIX_3D_H  1

#include <initializer_list>
#include <assert.h>

#include "util/matrixes.h"
#include "Exception.h"
#include "Types.h"

using namespace std;

namespace gcm
{
	/**
	 * @brief Base class to inherit rheology matrices implementations from.
	 * @details Holds rheology matrix A and provides a way to compute
	 *          «decomposition» of original matrix to 3 matrices U, L, U1 that
	 *          U1 * L * U == A.
	 */
	class RheologyMatrix3D
	{
	protected:
		gcm_matrix A;
		gcm_matrix L;
		gcm_matrix U;
		gcm_matrix U1;

		virtual void createAx(initializer_list<gcm_real> params) = 0;
		virtual void createAy(initializer_list<gcm_real> params) = 0;
		virtual void createAz(initializer_list<gcm_real> params) = 0;
	public:
		/**
		 * @brief Default constructor
		 */
		//RheologyMatrix3D();
		/**
		 * @brief Destructor
		 */
		virtual ~RheologyMatrix3D() = 0;
		/**
		 * @brief Prepares matrix using specified param values.
		 * @details Creates rheology matrices (Ax, Ay, Az) depending on stage
		 * 
		 * @param params Rheology parameters (meaning and number of parameters
		 *               depend on type of rheology)
		 * @param stage Calculation stage
		 */
		void prepareMatrix(initializer_list<gcm_real> params, unsigned int stage);
		/**
		 * @brief Returns max matrix eigenvalue.
		 * @return max eigenvalue.
		 */
		gcm_real getMaxEigenvalue();
		/**
		 * @brief Returns L matrix (contains eigenvalues).
		 * @return L matrix.
		 */
		gcm_matrix& getL();
		/**
		 * @brief Returns U matrix (contains eigenvectors).
		 * @return U matrix.
		 */
		gcm_matrix& getU();
		/**
		 * @brief Returns U1 matrix (inverse U1).
		 * @return U1 matrix.
		 */
		gcm_matrix& getU1();
	};
}

#endif
