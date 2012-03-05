#ifndef _GCM_TETRAHEDRON_H
#define _GCM_TETRAHEDRON_H  1

#include "Element.h"

class Tetrahedron : public Element
{
/* Inherited from Element
*	int local_num;
*	int absolute_num;
*/
public:
	int vert[4];
};

#endif
