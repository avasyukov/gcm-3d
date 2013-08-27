#ifndef GCM_DUMMY_RHEOLOGY_CALCULATOR_H_
#define GCM_DUMMY_RHEOLOGY_CALCULATOR_H_

#include "RheologyCalculator.h"
#include <string>
#include "../Logging.h"

using namespace std;

namespace gcm {
	class CalcNode;
	
	class DummyRheologyCalculator : public RheologyCalculator {
	public:
		DummyRheologyCalculator();
		~DummyRheologyCalculator();
		/*
		 * Returns rheology calculator type
		 */
		inline string getType() {
			return "DummyRheologyCalculator";
		}
		void doCalc(CalcNode* src, CalcNode* dst);
	protected:
		USE_LOGGER;
	};
}


#endif
