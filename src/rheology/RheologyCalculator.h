#ifndef GCM_RHEOLOGY_CALCULATOR_H_
#define GCM_RHEOLOGY_CALCULATOR_H_

#include <string>
#include "../node/ElasticNode.h"

using namespace std;
using namespace gcm;

namespace gcm {
	class RheologyCalculator {
	public:
		RheologyCalculator();
		~RheologyCalculator();
		/*
		 * Returns snapshot writer type
		 */
		inline string getType() {
			return "DummyRheology";
		}
		inline void doCalc(ElasticNode* src, ElasticNode* dst) {
			// do nothing
		}
	};
}


#endif
