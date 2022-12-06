#include "libgcm/failure/HoffmanFailureModel.hpp"

using namespace gcm;

gcm::HoffmanFailureModel::HoffmanFailureModel() {
	type = "HoffmanFailureModel";
	INIT_LOGGER( "gcm.HoffmanFailureModel" );
	criteria.push_back(new HoffmanFailureCriterion());
	correctors.push_back(new ZeroDeviatorFailedMaterialCorrector());
}
