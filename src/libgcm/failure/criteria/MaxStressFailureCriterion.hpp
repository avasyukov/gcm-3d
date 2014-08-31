#ifndef GCM_MaxStressFailureCriterion_H
#define GCM_MaxStressFailureCriterion_H

#include <string>

#include "libgcm/failure/criteria/FailureCriterion.hpp"

using namespace std;
using namespace gcm;

namespace gcm {

    class MaxStressFailureCriterion : public FailureCriterion {
    public:
        MaxStressFailureCriterion();
		void checkFailure(ICalcNode& node);
    protected:
        string type;
    private:
        USE_LOGGER;
    };
}

#endif

