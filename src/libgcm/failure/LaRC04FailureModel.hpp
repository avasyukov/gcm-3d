#ifndef GCM_LaRC04FailureModel_H
#define GCM_LaRC04FailureModel_H

#include "libgcm/failure/FailureModel.hpp"
#include "libgcm/failure/criteria/LaRC04FailureCriterion.hpp"
#include "libgcm/failure/correctors/ZeroDeviatorFailedMaterialCorrector.hpp"


namespace gcm {

	class LaRC04FailureModel : public FailureModel {
		public:
			LaRC04FailureModel();
		private:
			USE_LOGGER;
	};
}

#endif
