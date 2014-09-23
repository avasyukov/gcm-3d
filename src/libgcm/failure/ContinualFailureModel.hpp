#ifndef GCM_ContinualFailureModel_H
#define GCM_ContinualFailureModel_H

#include "libgcm/failure/FailureModel.hpp"
#include "libgcm/failure/criteria/MaxStressContinualFailureCriterion.hpp"

using namespace gcm;

namespace gcm {

    class ContinualFailureModel : public FailureModel {
    public:
        ContinualFailureModel();
    private:
        USE_LOGGER;
    };
}

#endif

