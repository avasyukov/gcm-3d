#ifndef _GCM_ELASTIC_MATRIX_3D_H
#define _GCM_ELASTIC_MATRIX_3D_H  1

#include <assert.h>

#include "util/matrixes.h"
#include "util/RheologyMatrix3D.h"
#include "Exception.h"

using namespace gcm;

namespace gcm {
	/**
	 * @brief Elastic rheology matrix implementation.
	 * @details Creates corresponding rheology matrices for case 
	 *          of elastic  material. Params in this case contain
	 *          lambda, mu and rho.
	 */
	class ElasticMatrix3D: public RheologyMatrix3D
	{
	protected:
		void clear();
		void getRheologyParameters(initializer_list<gcm_real> params, gcm_real& lambda, gcm_real& mu, gcm_real& rho);
		virtual void createAx(initializer_list<gcm_real> params) override;
		virtual void createAy(initializer_list<gcm_real> params) override;
		virtual void createAz(initializer_list<gcm_real> params) override;
	};
}

#endif
