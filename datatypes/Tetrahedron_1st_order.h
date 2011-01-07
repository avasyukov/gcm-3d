#ifndef _GCM_TETRAHEDRON_1ST_ORDER_H
#define _GCM_TETRAHEDRON_1ST_ORDER_H  1

#include "Element.h"

class Tetrahedron_1st_order : public Element
{
/* Inherited from Element
*	int local_num;
*	int absolute_num;
*/
public:
	int vert[4];
	// TODO How to deal with tmp elements???
	// float min_h;
	// ... and so on
};

#include "Tetrahedron_1st_order.inl"

#endif
