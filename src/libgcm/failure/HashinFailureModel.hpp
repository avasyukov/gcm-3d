#ifndef GCM_HashinFailureModel_H
#define GCM_HashinFailureModel_H

#include "libgcm/failure/FailureModel.hpp"
#include "libgcm/failure/criteria/HashinFailureCriterion.hpp"
#include "libgcm/failure/correctors/ZeroDeviatorFailedMaterialCorrector.hpp"


namespace gcm {

    class HashinFailureModel : public FailureModel {
    public:
        HashinFailureModel();
    private:
        USE_LOGGER;
    };
}

#endif

