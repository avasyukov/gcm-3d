#include "libgcm/failure/ContinualFailureModel.hpp"

using namespace gcm;

ContinualFailureModel::ContinualFailureModel() {
    type = "ContinualFailureModel";
    INIT_LOGGER( "gcm.ContinualFailureModel" );
    criteria.push_back(new MaxStressContinualFailureCriterion());
}
