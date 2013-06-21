/* 
 * File:   TetrFirstOrderInterpolator.h
 * Author: anganar
 *
 * Created on May 3, 2013, 12:13 AM
 */

#ifndef GCM_TETR_FIRST_ORDER_INTERPOLATOR_H
#define	GCM_TETR_FIRST_ORDER_INTERPOLATOR_H

#include <string>
#include "../node/ElasticNode.h"
#include "../Math.h"
#include "../Logging.h"
#include "TetrInterpolator.h"

using namespace std;
using namespace gcm;

namespace gcm {

	class TetrFirstOrderInterpolator : public TetrInterpolator {
	public:
		TetrFirstOrderInterpolator();
		~TetrFirstOrderInterpolator();
		void interpolate( ElasticNode* node, 
				ElasticNode* node0, ElasticNode* node1, ElasticNode* node2, ElasticNode* node3 );
	private:
		USE_LOGGER;
	};
}

#endif	/* GCM_TETR_FIRST_ORDER_INTERPOLATOR_H */

