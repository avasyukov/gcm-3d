#ifndef MaxStressContinualFailureCriterion_HPP
#define	MaxStressContinualFailureCriterion_HPP
 
#include <string>
#include <iostream>

#include "libgcm/failure/criteria/FailureCriterion.hpp"

using namespace std;
using namespace gcm;

namespace gcm {

    class MaxStressContinualFailureCriterion : public FailureCriterion {
    public:
        MaxStressContinualFailureCriterion();
		void checkFailure(ICalcNode& node, const float tau);
    protected:
        string type;
    private:
        USE_LOGGER;
    };
}

#endif

