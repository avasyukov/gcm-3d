#ifndef GCM_MaxStressCrackFailureCriterion_H
#define GCM_MaxStressCrackFailureCriterion_H

#include <string>

#include "libgcm/failure/criteria/FailureCriterion.hpp"


namespace gcm {

    class MaxStressCrackFailureCriterion : public FailureCriterion {
    public:
        MaxStressCrackFailureCriterion();
		void checkFailure(ICalcNode& node, const float tau);
    protected:
        std::string type;
    private:
        USE_LOGGER;
    };
}

#endif

