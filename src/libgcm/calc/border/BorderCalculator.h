/* 
 * File:   BorderCalculator.h
 * Author: anganar
 *
 * Created on April 30, 2013, 3:18 PM
 */

#ifndef GCM_BORDER_CALCULATOR_H_
#define GCM_BORDER_CALCULATOR_H_  1

#include <string>
#include "../../util/ElasticMatrix3D.h"
#include "../../Logging.h"

using namespace std;

namespace gcm
{
	class CalcNode;
	/*
	 * Base class for inner points calculators
	 */
	class BorderCalculator {
	public:
		/*
		 * Constructor
		 */
		BorderCalculator();
		/*
		 * Destructor
		 */
		~BorderCalculator();
		/*
		 * Calculate next state for the given node
		 */
		virtual void do_calc(CalcNode* new_node, ElasticMatrix3D* matrix, float* values[], bool inner[], float outer_normal[], float magnitude) = 0;
		/*
		 * Returns type of the calculator
		 */
		virtual string getType() = 0;
		
	private:
	};
	
}

#endif	/* GCM_BORDER_CALCULATOR_H_ */
