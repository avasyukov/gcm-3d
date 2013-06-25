#ifndef GCM_TRIANGLE_SECOND_ORDER_H_
#define GCM_TRIANGLE_SECOND_ORDER_H_

#include "TriangleFirstOrder.h"

namespace gcm {
	/*
	 * Triangle base class to inherit from.
	 */
	class TriangleSecondOrder : public TriangleFirstOrder {
	public:
		TriangleSecondOrder();
		TriangleSecondOrder(int n, int v[3]);
		TriangleSecondOrder(int n, int v[3], int av[3]);
		/*
		 * List of additional vertices on edges
		 */
		int addVerts[3];
	};
}
#endif