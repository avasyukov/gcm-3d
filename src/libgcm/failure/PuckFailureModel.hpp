#ifndef GCM_PuckFailureModel_H
#define GCM_PuckFailureModel_H

#include "libgcm/failure/FailureModel.hpp"
#include "libgcm/failure/criteria/PuckFailureCriterion.hpp"
#include "libgcm/failure/correctors/CrackResponseFailedMaterialCorrector.hpp"


namespace gcm {

    class PuckFailureModel : public FailureModel {
    public:
        PuckFailureModel();
    private:
        USE_LOGGER;
    };
}

#endif

