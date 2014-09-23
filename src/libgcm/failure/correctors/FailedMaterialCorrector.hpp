#ifndef GCM_FailedMaterialCorrector_H
#define GCM_FailedMaterialCorrector_H

#include <string>

#include "libgcm/Math.hpp"
#include "libgcm/Logging.hpp"

#include "libgcm/node/ICalcNode.hpp"


namespace gcm {

    class FailedMaterialCorrector {
    public:
        FailedMaterialCorrector();
        ~FailedMaterialCorrector();
		virtual void applyCorrection(ICalcNode& node, const float tau) = 0;
    private:
        USE_LOGGER;
    };
}

#endif

