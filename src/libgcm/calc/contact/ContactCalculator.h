/* 
 * File:   ContactCalculator.h
 * Author: anganar
 *
 * Created on April 30, 2013, 3:18 PM
 */

#ifndef GCM_CONTACT_CALCULATOR_H_
#define GCM_CONTACT_CALCULATOR_H_  1

#include <string>
#include "../../node/ElasticNode.h"
#include "../../util/ElasticMatrix3D.h"

using namespace std;

namespace gcm
{
	/*
	 * Base class for inner points calculators
	 */
	class ContactCalculator {
	public:
		/*
		 * Constructor
		 */
		ContactCalculator();
		/*
		 * Destructor
		 */
		~ContactCalculator();
		/*
		 * Calculate next state for the given node
		 */
		virtual void do_calc(ElasticNode* new_node, ElasticMatrix3D* matrix, float* values[], bool inner[], ElasticMatrix3D* virt_matrix, float* virt_values[], bool virt_inner[], float outer_normal[], float scale) = 0;
		/*
		 * Returns type of the calculator
		 */
		virtual string getType() = 0;
		
	private:
		
	};
	
}

#endif	/* GCM_CONTACT_CALCULATOR_H_ */
