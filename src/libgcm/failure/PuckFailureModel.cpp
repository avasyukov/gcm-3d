#include "libgcm/failure/PuckFailureModel.hpp"

using namespace gcm;

PuckFailureModel::PuckFailureModel() {
    type = "PuckFailureModel";
    INIT_LOGGER( "gcm.PuckFailureModel" );
    criteria.push_back(new PuckFailureCriterion());
    correctors.push_back(new CrackResponseFailedMaterialCorrector());
}
