#include "libgcm/failure/CrackFailureModel.hpp"

using namespace gcm;

CrackFailureModel::CrackFailureModel() {
    type = "CrackFailureModel";
    INIT_LOGGER( "gcm.CrackFailureModel" );
    criteria.push_back(new MaxStressCrackFailureCriterion());
    correctors.push_back(new CrackResponseFailedMaterialCorrector());
}
