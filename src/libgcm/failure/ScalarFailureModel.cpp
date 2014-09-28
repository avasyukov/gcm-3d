#include "libgcm/failure/ScalarFailureModel.hpp"

using namespace gcm;

ScalarFailureModel::ScalarFailureModel() {
    type = "ScalarFailureModel";
    INIT_LOGGER( "gcm.ScalarFailureModel" );
    criteria.push_back(new MaxStressFailureCriterion());
    correctors.push_back(new ZeroDeviatorFailedMaterialCorrector());
}
