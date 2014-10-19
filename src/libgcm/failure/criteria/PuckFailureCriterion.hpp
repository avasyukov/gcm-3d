#ifndef GCM_PuckFailureCriterion_H
#define GCM_PuckFailureCriterion_H

#include <string>

#include "libgcm/failure/criteria/FailureCriterion.hpp"


namespace gcm {

    class PuckFailureCriterion : public FailureCriterion {
    public:
        PuckFailureCriterion();
		void checkFailure(ICalcNode& node, const float tau);
    protected:
        std::string type;
    private:
        USE_LOGGER;
    };
}

#endif

