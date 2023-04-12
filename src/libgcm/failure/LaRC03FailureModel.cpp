#include "libgcm/failure/LaRC03FailureModel.hpp"

using namespace gcm;

gcm::LaRC03FailureModel::LaRC03FailureModel() {
	type = "LaRC03FailureModel";
	INIT_LOGGER( "gcm.LaRC03FailureModel" );
	criteria.push_back(new LaRC03FailureCriterion());
	correctors.push_back(new ZeroDeviatorFailedMaterialCorrector());
}
