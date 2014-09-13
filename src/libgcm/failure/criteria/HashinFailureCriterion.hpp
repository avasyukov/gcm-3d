#ifndef GCM_HashinFailureCriterion_H
#define GCM_HashinFailureCriterion_H

#include <string>

#include "libgcm/failure/criteria/FailureCriterion.hpp"

using namespace std;
using namespace gcm;

namespace gcm {

    class HashinFailureCriterion : public FailureCriterion {
    public:
        HashinFailureCriterion();
		void checkFailure(ICalcNode& node, const float tau);
    protected:
        string type;
    private:
        USE_LOGGER;
    };
}

#endif

