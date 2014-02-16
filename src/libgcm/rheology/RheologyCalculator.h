#ifndef GCM_RHEOLOGY_CALCULATOR_H_
#define GCM_RHEOLOGY_CALCULATOR_H_

#include <string>

using namespace std;

namespace gcm {
	class CalcNode;
	
	class RheologyCalculator {
	public:
		virtual ~RheologyCalculator() = 0;
		/*
		 * Returns snapshot writer type
		 */
		virtual string getType() = 0;
		virtual void doCalc(CalcNode& src, CalcNode& dst) = 0;
	};
}


#endif
