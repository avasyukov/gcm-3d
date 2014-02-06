#ifndef _GCM_AREA_SPHERE_H
#define _GCM_AREA_SPHERE_H 1

#include "util/areas/Area.h"

class SphereArea : public Area
{
public:
	SphereArea( float _radius, float x, float y, float z );
	~SphereArea();
	bool isInArea( Node* cur_node );
private:
	float radius;
	float center[3];
};

#endif
