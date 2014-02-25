#ifndef _GCM_ANISOTROPIC_MATRIX_3D_H
#define _GCM_ANISOTROPIC_MATRIX_3D_H  1

#include <assert.h>

#include "util/matrixes.h"
#include "util/RheologyMatrix3D.h"
#include "Exception.h"

#include <gsl/gsl_eigen.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>

using namespace gcm;

namespace gcm {
	/**
	 * @brief Anisotropic rheology matrix implementation.
	 * @details Creates corresponding rheology matrices for case 
	 *          of anisotropic material. Params in this case contain
	 *          special structure 'AnisotropicNumbers' (at matrixes.h).
	 */
	class AnisotropicMatrix3D: public RheologyMatrix3D
	{
	protected:
		void clear();
		void getRheologyParameters(initializer_list<gcm_real> params, AnisotropicNumbers& numbers, gcm_real& rho);
		void decompositeIt(gsl_matrix* a, gsl_matrix* u, gsl_matrix* l, gsl_matrix* u1);
		virtual void createAx(initializer_list<gcm_real> params) override;
		virtual void createAy(initializer_list<gcm_real> params) override;
		virtual void createAz(initializer_list<gcm_real> params) override;
		
	private:
		void gcmTogsl(const gcm_matrix &a, gsl_matrix* b);
		void gslTogcm(gsl_matrix* a, gcm_matrix& b);
		void clear(gsl_matrix* a);
		void realChecker(gsl_vector_complex* a, gsl_matrix* l);
		void realChecker(gsl_matrix_complex* a, gsl_matrix* u);
	};
}

#endif
