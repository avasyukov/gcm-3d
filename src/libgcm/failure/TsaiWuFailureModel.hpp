#ifndef GCM_TsaiWuFailureModel_H
#define GCM_TsaiWuFailureModel_H

#include "libgcm/failure/FailureModel.hpp"
#include "libgcm/failure/criteria/TsaiWuFailureCriterion.hpp"
#include "libgcm/failure/correctors/ZeroDeviatorFailedMaterialCorrector.hpp"


namespace gcm {

	class TsaiWuFailureModel : public FailureModel {
		public:
			TsaiWuFailureModel();
		private:
			USE_LOGGER;
	};
}

#endif
