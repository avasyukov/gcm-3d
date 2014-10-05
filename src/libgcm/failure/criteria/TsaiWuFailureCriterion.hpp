#ifndef GCM_TsaiWuFailureCriterion_H
#define GCM_TsaiWuFailureCriterion_H

#include <string>
#include "libgcm/failure/criteria/FailureCriterion.hpp"

namespace gcm {
	class TsaiWuFailureCriterion : public FailureCriterion {
		public:
			TsaiWuFailureCriterion();
			void checkFailure(ICalcNode& node, const float tau);
		protected:
			std::string type;
		private:
			USE_LOGGER;
	};
}
#endif
