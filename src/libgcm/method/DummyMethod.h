#ifndef GCM_DUMMY_METHOD_H
#define	GCM_DUMMY_METHOD_H

#include "NumericalMethod.h"

namespace gcm {
	/*
	 * Numerical method
	 */
	class DummyMethod : public NumericalMethod {
	public:
		/*
		 * Constructor
		 */
		DummyMethod();
		/*
		 * Destructor
		 */
		~DummyMethod();
		/*
		 * Returns number of stages (part steps) this particular method requires
		 */
		int getNumberOfStages();
		/*
		 * Computes next state (after the next part step) for the given node
		 */
		void doNextPartStep(CalcNode* cur_node, CalcNode* new_node, float time_step, int stage, Mesh* mesh);
		/*
		 * Returns maximum lambda for the given node
		 */
		float getMaxLambda(CalcNode* node);
		
		string getType();
		
	protected:
		USE_LOGGER;
	};
}

#endif	/* GCM_DUMMY_METHOD_H */