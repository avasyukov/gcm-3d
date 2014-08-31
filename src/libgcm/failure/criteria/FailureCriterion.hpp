#ifndef GCM_FailureCriterion_H
#define GCM_FailureCriterion_H

#include <string>

#include "libgcm/Math.hpp"
#include "libgcm/Logging.hpp"

#include "libgcm/node/ICalcNode.hpp"

using namespace std;
using namespace gcm;

namespace gcm {

    class FailureCriterion {
    public:
        FailureCriterion();
        ~FailureCriterion();
		virtual void checkFailure(ICalcNode& node) = 0;
    private:
        USE_LOGGER;
    };
}

#endif

