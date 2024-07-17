#ifndef _GCM_AREA_HS_H
#define _GCM_AREA_HS_H 1

#include "libgcm/util/areas/Area.hpp"

namespace gcm {
	class HalfSpaceArea : public Area
	{
	public:
		~HalfSpaceArea();
		HalfSpaceArea( float x, float y, float z, float nx, float ny, float nz );
		bool isInArea(const Node& cur_node ) const;
	private:
		float x, y, z;
		float nx, ny, nz;
	};
}

#endif
