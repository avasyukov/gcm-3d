#ifndef _GCM_AREA_H
#define _GCM_AREA_H 1

#include "../../node/Node.h"
#include "../../Exception.h"
#include "../../Math.h"

class Area
{
public:
	virtual bool isInArea( Node* cur_node ) = 0;
};

#endif
