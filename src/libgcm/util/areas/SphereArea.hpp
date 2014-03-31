#ifndef _GCM_AREA_SPHERE_H
#define _GCM_AREA_SPHERE_H 1

#include "libgcm/util/areas/Area.hpp"

class SphereArea : public Area
{
public:
    SphereArea( float _radius, float x, float y, float z );
    bool isInArea( Node& cur_node );
private:
    float radius;
    float center[3];
};

#endif
