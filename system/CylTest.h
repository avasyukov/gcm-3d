#ifndef _GCM_CYLTEST_H
#define _GCM_CYLTEST_H  1

//-----------------------------------------------------------------------------
// Name: CylTest_CapsFirst
// Orig: Greg James - gjames@NVIDIA.com
// Lisc: Free code - no warranty & no money back.  Use it all you want
// Desc: 
//    This function tests if the 3D point 'testpt' lies within an arbitrarily
// oriented cylinder.  The cylinder is defined by an axis from 'pt1' to 'pt2',
// the axis having a length squared of 'lengthsq' (pre-compute for each cylinder
// to avoid repeated work!), and radius squared of 'radius_sq'.
//    The function tests against the end caps first, which is cheap -> only 
// a single dot product to test against the parallel cylinder caps.  If the
// point is within these, more work is done to find the distance of the point
// from the cylinder axis.
//    Fancy Math (TM) makes the whole test possible with only two dot-products
// a subtract, and two multiplies.  For clarity, the 2nd mult is kept as a
// divide.  It might be faster to change this to a mult by also passing in
// 1/lengthsq and using that instead.
//    Elminiate the first 3 subtracts by specifying the cylinder as a base
// point on one end cap and a vector to the other end cap (pass in {dx,dy,dz}
// instead of 'pt2' ).
//
//    The dot product is constant along a plane perpendicular to a vector.
//    The magnitude of the cross product divided by one vector length is
// constant along a cylinder surface defined by the other vector as axis.
//
// Return:  -1.0 if point is outside the cylinder
// Return:  distance squared from cylinder axis if point is inside.
//
//-----------------------------------------------------------------------------

#include <math.h>

class Vec3
{
public:
	float x;
	float y;
	float z;
	Vec3();
	Vec3( float _x, float _y, float _z );
	Vec3& operator=( const Vec3& rhs );
	float operator*( const Vec3& rhs );
	Vec3 operator*( float a );
	Vec3 operator-( const Vec3& rhs );
	Vec3 operator+( const Vec3& rhs );
	void normalize();
	Vec3 Norm_Project_Point_Line( Vec3 & c, Vec3 & a, Vec3 & b );
	float CylTest_CapsFirst( const Vec3 & pt1, const Vec3 & pt2, float lengthsq, float radius_sq, const Vec3 & testpt );
};

#endif
