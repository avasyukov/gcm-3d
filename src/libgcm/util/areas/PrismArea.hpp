#ifndef _GCM_AREA_PRISM_H
#define _GCM_AREA_PRISM_H 1

#include "libgcm/util/areas/Area.hpp"

namespace gcm {
	class PrismArea : public Area
	{
	public:
		PrismArea( float _p0[3], float _p1[3], float _p2[3], float _p3[3] );
		bool isInArea(const Node& cur_node ) const;
	private:
		float Points[4][3];
	};
}


#endif /* GCM_AREA_PRISM */
