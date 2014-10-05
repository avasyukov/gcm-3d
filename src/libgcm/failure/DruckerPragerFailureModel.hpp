#ifndef GCM_DruckerPragerFailureModel_H
#define GCM_DruckerPragerFailureModel_H

#include "libgcm/failure/FailureModel.hpp"
#include "libgcm/failure/criteria/DruckerPragerFailureCriterion.hpp"
#include "libgcm/failure/correctors/ZeroDeviatorFailedMaterialCorrector.hpp"


namespace gcm {

    class DruckerPragerFailureModel : public FailureModel {
    public:
        DruckerPragerFailureModel();
    private:
        USE_LOGGER;
    };
}

#endif

