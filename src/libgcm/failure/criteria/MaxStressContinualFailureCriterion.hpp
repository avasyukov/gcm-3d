#ifndef MaxStressContinualFailureCriterion_HPP
#define	MaxStressContinualFailureCriterion_HPP
 
#include <string>
#include <iostream>

#include "libgcm/failure/criteria/FailureCriterion.hpp"

namespace gcm {

    class MaxStressContinualFailureCriterion : public FailureCriterion {
    public:
        MaxStressContinualFailureCriterion();
		void checkFailure(ICalcNode& node, const float tau);
    protected:
        std::string type;
    private:
        USE_LOGGER;
    };
}

#endif

