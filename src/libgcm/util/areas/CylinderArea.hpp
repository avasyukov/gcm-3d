#ifndef _GCM_AREA_CYLINDER_H
#define _GCM_AREA_CYLINDER_H 1

#include "libgcm/util/areas/Area.hpp"

namespace gcm {
	class CylinderArea : public Area
	{
	public:
		CylinderArea( float _radius, float _bX, float _bY, float _bZ, float _eX, float _eY, float _eZ );
		bool isInArea( const Node& cur_node ) const;
	private:
		float radius;
		float begin[3];
		float end[3];
	};
}

#endif
