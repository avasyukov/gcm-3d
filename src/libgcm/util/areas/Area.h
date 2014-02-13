#ifndef _GCM_AREA_H
#define _GCM_AREA_H 1

#include "node/Node.h"
#include "Exception.h"
#include "Math.h"

namespace gcm {
	class Area
	{
	public:
		virtual ~Area() = 0;
		virtual bool isInArea( Node* cur_node ) = 0;
	};
}

#endif
