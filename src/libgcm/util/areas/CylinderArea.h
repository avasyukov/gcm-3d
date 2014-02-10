#ifndef _GCM_AREA_CYLINDER_H
#define _GCM_AREA_CYLINDER_H 1

#include "util/areas/Area.h"

class CylinderArea : public Area
{
public:
	CylinderArea( float _radius, float _bX, float _bY, float _bZ, float _eX, float _eY, float _eZ );
	bool isInArea( Node* cur_node );
private:
	float radius;
	float begin[3];
	float end[3];
};

#endif
