#ifndef GCM_NoFailureModel_H
#define GCM_NoFailureModel_H

#include "libgcm/failure/FailureModel.hpp"

using namespace std;
using namespace gcm;

namespace gcm {

    class NoFailureModel : public FailureModel {
    public:
        NoFailureModel();
    private:
        USE_LOGGER;
    };
}

#endif

