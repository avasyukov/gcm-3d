#include "libgcm/failure/DruckerPragerFailureModel.hpp"

using namespace gcm;

DruckerPragerFailureModel::DruckerPragerFailureModel() {
    type = "DruckerPragerFailureModel";
    INIT_LOGGER( "gcm.DruckerPragerFailureModel" );
    criteria.push_back(new DruckerPragerFailureCriterion());
    correctors.push_back(new ZeroDeviatorFailedMaterialCorrector());
}
