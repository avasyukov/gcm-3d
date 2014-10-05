#ifndef GCM_DruckerPragerFailureCriterion_H
#define GCM_DruckerPragerFailureCriterion_H

#include <string>

#include "libgcm/failure/criteria/FailureCriterion.hpp"


namespace gcm {

    class DruckerPragerFailureCriterion : public FailureCriterion {
    public:
        DruckerPragerFailureCriterion();
		void checkFailure(ICalcNode& node, const float tau);
    protected:
        std::string type;
    private:
        USE_LOGGER;
    };
}

#endif

