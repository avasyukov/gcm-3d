#ifndef GCM_NoFailureModel_H
#define GCM_NoFailureModel_H

#include "libgcm/failure/FailureModel.hpp"


namespace gcm {

    class NoFailureModel : public FailureModel {
    public:
        NoFailureModel();
    private:
        USE_LOGGER;
    };
}

#endif

