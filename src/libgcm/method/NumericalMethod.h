/* 
 * File:   NumericalMethod.h
 * Author: anganar
 *
 * Created on April 30, 2013, 8:18 PM
 */

#ifndef GCM_NUMERICAL_METHOD_H
#define	GCM_NUMERICAL_METHOD_H

#include <string>

#include "Interfaces.h"
#include "mesh/Mesh.h"

namespace gcm {
	class CalcNode;
	/*
	 * Numerical method
	 */
	class NumericalMethod {
	public:
		/*
		 * Constructor
		 */
		NumericalMethod();
		/*
		 * Destructor
		 */
		~NumericalMethod();
		/*
		 * Returns number of stages (part steps) this particular method requires
		 */
		virtual int getNumberOfStages() = 0;
		/*
		 * Computes next state (after the next part step) for the given node
		 */
		virtual void doNextPartStep(CalcNode& cur_node, CalcNode& new_node, float time_step, int stage, Mesh* mesh) = 0;
		/*
		 * Returns maximum lambda for the given node
		 * TODO@avasyukov: Should we replace it with getMaxTimeStep?
		 */
		virtual float getMaxLambda(CalcNode* node) = 0; //(CalcNode* node, TetrMesh* mesh)
		
		virtual string getType() = 0;
		
		void setSpaceOrder(int order);
		void setTimeOrder(int order);
		int getSpaceOrder();
		int getTimeOrder();
		
	protected:
		int spaceOrder;
		int timeOrder;
	};
}

#endif	/* GCM_NUMERICAL_METHOD_H */
