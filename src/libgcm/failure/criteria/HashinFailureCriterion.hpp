#ifndef GCM_HashinFailureCriterion_H
#define GCM_HashinFailureCriterion_H

#include <string>

#include "libgcm/failure/criteria/FailureCriterion.hpp"


namespace gcm {

    class HashinFailureCriterion : public FailureCriterion {
    public:
        HashinFailureCriterion();
		void checkFailure(ICalcNode& node, const float tau);
    protected:
        std::string type;
    private:
        USE_LOGGER;
    };
}

#endif

