#ifndef GCM_DUMMY_METHOD_H
#define	GCM_DUMMY_METHOD_H

#include "method/NumericalMethod.h"

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
		 * Returns number of stages (part steps) this particular method requires
		 */
		int getNumberOfStages();
		/*
		 * Computes next state (after the next part step) for the given node
		 */
		void doNextPartStep(CalcNode& cur_node, CalcNode& new_node, float time_step, int stage, Mesh* mesh);
		
		string getType();
		
	protected:
		USE_LOGGER;
	};
}

#endif	/* GCM_DUMMY_METHOD_H */
