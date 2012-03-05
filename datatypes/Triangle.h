#ifndef _GCM_TRIANGLE_H
#define _GCM_TRIANGLE_H  1

#include "Element.h"

class Triangle : public Element
{
/* Inherited from Element
*	int local_num;
*	int absolute_num;
*/
public:
	int vert[3];
};

#endif
