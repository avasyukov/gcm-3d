#ifndef GCM_MaxStressCrackFailureCriterion_H
#define GCM_MaxStressCrackFailureCriterion_H

#include <string>

#include "libgcm/failure/criteria/FailureCriterion.hpp"

using namespace std;
using namespace gcm;

namespace gcm {

    class MaxStressCrackFailureCriterion : public FailureCriterion {
    public:
        MaxStressCrackFailureCriterion();
		void checkFailure(ICalcNode& node, const float tau);
    protected:
        string type;
    private:
        USE_LOGGER;
    };
}

#endif

