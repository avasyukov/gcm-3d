#include "libgcm/failure/TsaiHillFailureModel.hpp"

using namespace gcm;

gcm::TsaiHillFailureModel::TsaiHillFailureModel() {
	type = "TsaiHillFailureModel";
	INIT_LOGGER( "gcm.TsaiHillFailureModel" );
	criteria.push_back(new TsaiHillFailureCriterion());
	correctors.push_back(new ZeroDeviatorFailedMaterialCorrector());
}
