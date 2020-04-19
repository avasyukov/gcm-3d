#ifndef GCM_FailureModel_H
#define GCM_FailureModel_H

#include <string>
#include <vector>

#include "libgcm/Math.hpp"
#include "libgcm/Logging.hpp"

#include "libgcm/failure/criteria/FailureCriterion.hpp"
#include "libgcm/failure/correctors/FailedMaterialCorrector.hpp"


namespace gcm {

	class ICalcNode;
	class CalcNode;

    class FailureModel {
    public:
        FailureModel();
        ~FailureModel();
        std::string getType();
		void checkFailure(ICalcNode& node, const float tau);
		void applyCorrection(ICalcNode& node, const float tau);

    protected:
        std::string type;
		std::vector<FailureCriterion*> criteria;
		std::vector<FailedMaterialCorrector*> correctors;

		CalcNode prepare_node(ICalcNode& node);
    private:
        USE_LOGGER;
    };
}

#endif

