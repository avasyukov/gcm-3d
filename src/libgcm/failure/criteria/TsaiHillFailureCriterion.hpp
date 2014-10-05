#ifndef GCM_TsaiHillFailureCriterion_H
#define GCM_TsaiHillFailureCriterion_H

#include <string>
#include "libgcm/failure/criteria/FailureCriterion.hpp"

namespace gcm {
	class TsaiHillFailureCriterion : public FailureCriterion {
		public:
			TsaiHillFailureCriterion();
			void checkFailure(ICalcNode& node, const float tau);
		protected:
			std::string type;
		private:
			USE_LOGGER;
	};
}

#endif
