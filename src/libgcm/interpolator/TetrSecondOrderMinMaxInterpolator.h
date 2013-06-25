/* 
 * File:   TetrSecondOrderMinMaxInterpolator.h
 * Author: anganar
 *
 * Created on May 3, 2013, 12:13 AM
 */

#ifndef GCM_TETR_SECOND_ORDER_MINMAX_INTERPOLATOR_H
#define	GCM_TETR_SECOND_ORDER_MINMAX_INTERPOLATOR_H

#include <string>
#include "../node/ElasticNode.h"
#include "../Math.h"
#include "../Logging.h"
#include "TetrInterpolator.h"

using namespace std;
using namespace gcm;

namespace gcm {

	class TetrSecondOrderMinMaxInterpolator : public TetrInterpolator {
	public:
		TetrSecondOrderMinMaxInterpolator();
		~TetrSecondOrderMinMaxInterpolator();
		void interpolate( ElasticNode* node, 
				ElasticNode* node0, ElasticNode* node1, ElasticNode* node2, ElasticNode* node3,
				ElasticNode* addNode0, ElasticNode* addNode1, ElasticNode* addNode2, 
				ElasticNode* addNode3, ElasticNode* addNode4, ElasticNode* addNode5 );
	private:
		ElasticNode* baseNodes[4];
		ElasticNode* addNodes[6];
		USE_LOGGER;
	};
}

#endif	/* GCM_TETR_FIRST_ORDER_INTERPOLATOR_H */

