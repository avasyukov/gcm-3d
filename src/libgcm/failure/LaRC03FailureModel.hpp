#ifndef GCM_LaRC03FailureModel_H
#define GCM_LaRC03FailureModel_H

#include "libgcm/failure/FailureModel.hpp"
#include "libgcm/failure/criteria/LaRC03FailureCriterion.hpp"
#include "libgcm/failure/correctors/ZeroDeviatorFailedMaterialCorrector.hpp"


namespace gcm {

	class LaRC03FailureModel : public FailureModel {
		public:
			LaRC03FailureModel();
		private:
			USE_LOGGER;
	};
}

#endif
