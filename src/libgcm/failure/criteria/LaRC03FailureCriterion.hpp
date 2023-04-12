#ifndef GCM_LaRC03FailureCriterion_H
#define GCM_LaRC03ailureCriterion_H

#include <string>

#include "libgcm/failure/criteria/FailureCriterion.hpp"


namespace gcm {

    class LaRC03FailureCriterion : public FailureCriterion {
    public:
        LaRC03FailureCriterion();
		void checkFailure(ICalcNode& node, const float tau);
    protected:
        std::string type;
    private:
        USE_LOGGER;
    };
}

#endif

