#ifndef CONTINUALDAMAGEFAILURECRITERION_HPP
#define	CONTINUALDAMAGEFAILURECRITERION_HPP
 
#include <string>
#include <iostream>

#include "libgcm/failure/criteria/FailureCriterion.hpp"

using namespace std;
using namespace gcm;

namespace gcm {

    class ContinualDamageFailureCriterion : public FailureCriterion {
    public:
        ContinualDamageFailureCriterion();
		void checkFailure(ICalcNode& node, const float tau);
    protected:
        string type;
    private:
        USE_LOGGER;
    };
}

#endif

