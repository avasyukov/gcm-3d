#include "libgcm/util/areas/CylinderArea.hpp"

CylinderArea::CylinderArea( float _radius, float _bX, float _bY, float _bZ, float _eX, float _eY, float _eZ )
{
    radius = _radius;
    begin[0] = _bX;
    begin[1] = _bY;
    begin[2] = _bZ;

    end[0] = _eX;
    end[1] = _eY;
    end[2] = _eZ;
};

// Based on http://www.flipcode.com/archives/Fast_Point-In-Cylinder_Test.shtml
// From the original comments:
//-----------------------------------------------------------------------------
//    The function tests against the end caps first, which is cheap -> only 
// a single dot product to test against the parallel cylinder caps.  If the
// point is within these, more work is done to find the distance of the point
// from the cylinder axis.
//    Fancy Math (TM) makes the whole test possible with only two dot-products
// a subtract, and two multiplies.  For clarity, the 2nd mult is kept as a
// divide.  It might be faster to change this to a mult by also passing in
// 1/lengthsq and using that instead.
//-----------------------------------------------------------------------------
bool CylinderArea::isInArea( Node& cur_node )
{
    gcm_real dx = end[0] - begin[0];
    gcm_real dy = end[1] - begin[1];
    gcm_real dz = end[2] - begin[2];
    
    gcm_real pdx = cur_node.coords[0] - begin[0];
    gcm_real pdy = cur_node.coords[1] - begin[1];
    gcm_real pdz = cur_node.coords[2] - begin[2];
    
    gcm_real lengthsq = dx * dx + dy * dy + dz * dz;
    gcm_real radius_sq = radius * radius;

    // Dot the d and pd vectors to see if point lies behind the 
    // cylinder cap at pt1.x, pt1.y, pt1.z

    gcm_real dot = pdx * dx + pdy * dy + pdz * dz;

    // If dot is less than zero the point is behind the pt1 cap.
    // If greater than the cylinder axis line segment length squared
    // then the point is outside the other end cap at pt2.

    if (dot < 0.0f || dot > lengthsq)
    {
        return false;
    }
    else
    {
        // Point lies within the parallel caps, so find
        // distance squared from point to line, using the fact that sin^2 + cos^2 = 1
        // the dot = cos() * |d||pd|, and cross*cross = sin^2 * |d|^2 * |pd|^2
        // Carefull: '*' means mult for scalars and dotproduct for vectors
        // In short, where dist is pt distance to cyl axis: 
        // dist = sin( pd to d ) * |pd|
        // distsq = dsq = (1 - cos^2( pd to d)) * |pd|^2
        // dsq = ( 1 - (pd * d)^2 / (|pd|^2 * |d|^2) ) * |pd|^2
        // dsq = pd * pd - dot * dot / lengthsq
        //  where lengthsq is d*d or |d|^2 that is passed into this function 

        // distance squared to the cylinder axis:

        gcm_real dsq = (pdx * pdx + pdy * pdy + pdz * pdz) - dot * dot / lengthsq;

        if (dsq > radius_sq) 
        {
            return false;
        }
        else
        {
            return true;
        }
    }
};
