#ifndef GCM_CrackFailureModel_H
#define GCM_CrackFailureModel_H

#include "libgcm/failure/FailureModel.hpp"
#include "libgcm/failure/criteria/MaxStressCrackFailureCriterion.hpp"
#include "libgcm/failure/correctors/CrackResponseFailedMaterialCorrector.hpp"

using namespace std;
using namespace gcm;

namespace gcm {

    class CrackFailureModel : public FailureModel {
    public:
        CrackFailureModel();
    private:
        USE_LOGGER;
    };
}

#endif

