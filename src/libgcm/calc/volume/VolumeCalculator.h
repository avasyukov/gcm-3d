/* 
 * File:   VolumeCalculator.h
 * Author: anganar
 *
 * Created on April 30, 2013, 3:18 PM
 */

#ifndef GCM_VOLUME_CALCULATOR_H_
#define GCM_VOLUME_CALCULATOR_H_  1

#include <string>
#include <vector>

#include "util/ElasticMatrix3D.h"

using namespace std;

namespace gcm
{
	class CalcNode;
	/*
	 * Base class for inner points calculators
	 */
	class VolumeCalculator {
	public:
		/*
		 * Constructor
		 */
		VolumeCalculator();
		/*
		 * Destructor
		 */
		~VolumeCalculator();
		/*
		 * Calculate next state for the given node
		 */
		virtual void doCalc(CalcNode& new_node, ElasticMatrix3D& matrix, 
												vector<CalcNode>& previousNodes) = 0;
		/*
		 * Returns type of the calculator
		 */
		virtual string getType() = 0;
		
	private:
		
	};
	
}

#endif	/* GCM_VOLUME_CALCULATOR_H_ */

