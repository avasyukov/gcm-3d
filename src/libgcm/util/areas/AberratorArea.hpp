#ifndef _GCM_AREA_ABERRATOR_H
#define _GCM_AREA_ABERRATOR_H 1

#include "libgcm/util/areas/Area.hpp"
#include <vector>

namespace gcm {
	class AberratorArea : public Area
	{
	public:
		~AberratorArea();
        AberratorArea( float _minX, float _maxX, float _minY, float _maxY, float _flatZ, std::vector<float> &_curve );
		bool isInArea(const Node& cur_node ) const;
	private:
	    // Aberrator is an area that is limited by a rectangle in XY plane, an XY plane and a curve
	    // defined by a vector. The plane can be above or below the curve.
	    // Curves are in the XZ plane, so cuts of the aberrator across the X axis are rectangles.
	    //                 _________________
	    //                /               /|
	    //               /               / |
	    //               ----------------  |       Z   Y
	    //               |              |  |       ^  /
	    //               | __           | /        | /
	    //               |/  \____--^-v\|/          -> X
	    //
		float minX, maxX;
		float minY, maxY;
		float flatZ;
		std::vector<float> curve;
		float getCurveForX(float x) const;
	};
}

#endif
