#ifndef _GCM_AREA_BOX_H
#define _GCM_AREA_BOX_H 1

#include "libgcm/util/areas/Area.hpp"

namespace gcm {
	class BoxArea : public Area
	{
	public:
		~BoxArea();
		BoxArea( float _minX, float _maxX, float _minY, float _maxY, float _minZ, float _maxZ );
		bool isInArea(const Node& cur_node ) const;
	//private:
		float minX, maxX;
		float minY, maxY;
		float minZ, maxZ;
	};
}

#endif
