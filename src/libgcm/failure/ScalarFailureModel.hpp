#ifndef GCM_ScalarFailureModel_H
#define GCM_ScalarFailureModel_H

#include "libgcm/failure/FailureModel.hpp"
#include "libgcm/failure/criteria/MaxStressFailureCriterion.hpp"
#include "libgcm/failure/correctors/ZeroDeviatorFailedMaterialCorrector.hpp"

using namespace std;
using namespace gcm;

namespace gcm {

    class ScalarFailureModel : public FailureModel {
    public:
        ScalarFailureModel();
    private:
        USE_LOGGER;
    };
}

#endif

