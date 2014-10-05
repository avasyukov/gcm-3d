#ifndef GCM_TsaiHillFailureModel_H
#define GCM_TsaiHillFailureModel_H

#include "libgcm/failure/FailureModel.hpp"
#include "libgcm/failure/criteria/TsaiHillFailureCriterion.hpp"
#include "libgcm/failure/correctors/ZeroDeviatorFailedMaterialCorrector.hpp"


namespace gcm {

	class TsaiHillFailureModel : public FailureModel {
		public:
			TsaiHillFailureModel();
		private:
			USE_LOGGER;
	};
}

#endif
