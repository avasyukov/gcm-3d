#ifndef GCM_FailureModel_H
#define GCM_FailureModel_H

#include <string>

#include "libgcm/Math.hpp"
#include "libgcm/Logging.hpp"

#include "libgcm/node/ICalcNode.hpp"
#include "libgcm/failure/criteria/FailureCriterion.hpp"
#include "libgcm/failure/correctors/FailedMaterialCorrector.hpp"

using namespace std;
using namespace gcm;

namespace gcm {

    class FailureModel {
    public:
        FailureModel();
        ~FailureModel();
        string getType();
		void checkFailure(ICalcNode& node, const float tau);
		void applyCorrection(ICalcNode& node, const float tau);
    protected:
        string type;
		vector<FailureCriterion*> criteria;
		vector<FailedMaterialCorrector*> correctors;
    private:
        USE_LOGGER;
    };
}

#endif

