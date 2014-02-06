#ifndef GCM_STD_RHEOLOGY_CALCULATOR_H_
#define GCM_STD_RHEOLOGY_CALCULATOR_H_

#include <string>

#include "rheology/RheologyCalculator.h"
#include "Logging.h"

using namespace std;

namespace gcm {
	class CalcNode;
	
	class StdRheologyCalculator : public RheologyCalculator {
	public:
		StdRheologyCalculator();
		~StdRheologyCalculator();
		/*
		 * Returns rheology calculator type
		 */
		inline string getType() {
			return "StdRheologyCalculator";
		}
		void doCalc(CalcNode* src, CalcNode* dst);
	protected:
		USE_LOGGER;
	};
}


#endif
