#include "libgcm/failure/LaRC04FailureModel.hpp"

using namespace gcm;

gcm::LaRC04FailureModel::LaRC04FailureModel() {
	type = "LaRC04FailureModel";
	INIT_LOGGER( "gcm.LaRC04FailureModel" );
	criteria.push_back(new LaRC04FailureCriterion());
	correctors.push_back(new ZeroDeviatorFailedMaterialCorrector());
}
