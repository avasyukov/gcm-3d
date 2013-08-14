#ifndef GCM_RHEOLOGY_CALCULATOR_H_
#define GCM_RHEOLOGY_CALCULATOR_H_

#include <string>

using namespace std;

namespace gcm {
	class CalcNode;
	
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
		inline void doCalc(CalcNode* src, CalcNode* dst) {
			// do nothing
		}
	};
}


#endif
