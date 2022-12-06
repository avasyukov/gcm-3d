#ifndef GCM_HoffmanFailureModel_H
#define GCM_HoffmanFailureModel_H

#include "libgcm/failure/FailureModel.hpp"
#include "libgcm/failure/criteria/HoffmanFailureCriterion.hpp"
#include "libgcm/failure/correctors/ZeroDeviatorFailedMaterialCorrector.hpp"


namespace gcm {

	class HoffmanFailureModel : public FailureModel {
		public:
			HoffmanFailureModel();
		private:
			USE_LOGGER;
	};
}

#endif
