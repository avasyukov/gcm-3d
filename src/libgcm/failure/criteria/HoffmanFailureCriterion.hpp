#ifndef GCM_HoffmanFailureCriterion_H
#define GCM_HoffmanFailureCriterion_H

#include <string>
#include "libgcm/failure/criteria/FailureCriterion.hpp"

namespace gcm {
	class HoffmanFailureCriterion : public FailureCriterion {
		public:
			HoffmanFailureCriterion();
			void checkFailure(ICalcNode& node, const float tau);
		protected:
			std::string type;
		private:
			USE_LOGGER;
	};
}

#endif
