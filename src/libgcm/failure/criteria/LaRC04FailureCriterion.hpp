#ifndef GCM_LaRC04FailureCriterion_H
#define GCM_LaRC04ailureCriterion_H

#include <string>

#include "libgcm/failure/criteria/FailureCriterion.hpp"


namespace gcm {

    class LaRC04FailureCriterion : public FailureCriterion {
    public:
        LaRC04FailureCriterion();
		void checkFailure(ICalcNode& node, const float tau);
    protected:
        std::string type;
    private:
        USE_LOGGER;
    };
}

#endif

