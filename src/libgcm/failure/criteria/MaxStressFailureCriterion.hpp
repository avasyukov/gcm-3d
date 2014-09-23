#ifndef GCM_MaxStressFailureCriterion_H
#define GCM_MaxStressFailureCriterion_H

#include <string>

#include "libgcm/failure/criteria/FailureCriterion.hpp"


namespace gcm {

    class MaxStressFailureCriterion : public FailureCriterion {
    public:
        MaxStressFailureCriterion();
		void checkFailure(ICalcNode& node, const float tau);
    protected:
        std::string type;
    private:
        USE_LOGGER;
    };
}

#endif

