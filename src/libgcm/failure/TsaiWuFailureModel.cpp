#include "libgcm/failure/TsaiWuFailureModel.hpp"

using namespace gcm;

gcm::TsaiWuFailureModel::TsaiWuFailureModel() {
	type = "TsaiWuFailureModel";
	INIT_LOGGER( "gcm.TsaiWuFailureModel" );
	criteria.push_back(new TsaiWuFailureCriterion());
	correctors.push_back(new ZeroDeviatorFailedMaterialCorrector());
}
