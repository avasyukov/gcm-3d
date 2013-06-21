#ifndef _GCM_AREA_BOX_H
#define _GCM_AREA_BOX_H 1

#include "./Area.h"

class BoxArea : public Area
{
public:
	BoxArea( float _minX, float _maxX, float _minY, float _maxY, float _minZ, float _maxZ );
	~BoxArea();
	bool isInArea( Node* cur_node );
private:
	float minX, maxX;
	float minY, maxY;
	float minZ, maxZ;
};

#endif
