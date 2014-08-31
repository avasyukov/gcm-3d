#include "libgcm/failure/DebugFailureModel.hpp"

gcm::DebugFailureModel::DebugFailureModel() {
    type = "DebugFailureModel";
    INIT_LOGGER( "gcm.DebugFailureModel" );
    criteria.push_back(new MaxStressFailureCriterion());
    correctors.push_back(new DummyFailedMaterialCorrector());
}