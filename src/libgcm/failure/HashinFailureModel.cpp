#include "libgcm/failure/HashinFailureModel.hpp"

gcm::HashinFailureModel::HashinFailureModel() {
    type = "HashinFailureModel";
    INIT_LOGGER( "gcm.HashinFailureModel" );
    criteria.push_back(new HashinFailureCriterion());
    correctors.push_back(new ZeroDeviatorFailedMaterialCorrector());
}
