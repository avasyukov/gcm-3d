#include "CylTest.h"

Vec3::Vec3()
{
	x = 0;
	y = 0;
	z = 0;
}

Vec3::Vec3( float _x, float _y, float _z )
{
	x = _x;
	y = _y;
	z = _z;
}

Vec3& Vec3::operator=( const Vec3& rhs )
{
	if( this == &rhs ) {
		return *this;
	}
	
	this->x = rhs.x;
	this->y = rhs.y;
	this->z = rhs.z;
	
	return *this;
}

float Vec3::operator*( const Vec3& rhs )
{
	return x * rhs.x + y * rhs.y + z * rhs.z; 
}

Vec3 Vec3::operator*( float a )
{
	Vec3 ret;
	ret = (*this);
	ret.x *= a;
	ret.y *= a;
	ret.z *= a;
	
	return ret;
}

Vec3 Vec3::operator-( const Vec3& rhs )
{
	Vec3 ret;
	ret.x = x - rhs.x;
	ret.y = y - rhs.y;
	ret.z = z - rhs.z;
	
	return ret;
}

Vec3 Vec3::operator+( const Vec3& rhs )
{
	Vec3 ret;
	ret.x = x + rhs.x;
	ret.y = y + rhs.y;
	ret.z = z + rhs.z;
	
	return ret;
}

void Vec3::normalize()
{
	float l = sqrt( x * x + y * y + z * z );
	x /= l;
	y /= l;
	z /= l;
}

Vec3 Vec3::Norm_Project_Point_Line( Vec3 & c, Vec3 & a, Vec3 & b )
{
	Vec3 d = ( b - a );
	float l = ( ( c - a ) * d ) / ( d * d );
	
	Vec3 h = c - a - d * l;
	h.normalize();
	
	return h;
}

float Vec3::CylTest_CapsFirst( const Vec3 & pt1, const Vec3 & pt2, float lengthsq, float radius_sq, const Vec3 & testpt )
{
	float dx, dy, dz;	// vector d  from line segment point 1 to point 2
	float pdx, pdy, pdz;	// vector pd from point 1 to test point
	float dot, dsq;

	dx = pt2.x - pt1.x;	// translate so pt1 is origin.  Make vector from
	dy = pt2.y - pt1.y;     // pt1 to pt2.  Need for this is easily eliminated
	dz = pt2.z - pt1.z;

	pdx = testpt.x - pt1.x;		// vector from pt1 to test point.
	pdy = testpt.y - pt1.y;
	pdz = testpt.z - pt1.z;

	// Dot the d and pd vectors to see if point lies behind the 
	// cylinder cap at pt1.x, pt1.y, pt1.z

	dot = pdx * dx + pdy * dy + pdz * dz;

	// If dot is less than zero the point is behind the pt1 cap.
	// If greater than the cylinder axis line segment length squared
	// then the point is outside the other end cap at pt2.

	if( dot < 0.0f || dot > lengthsq )
	{
		return( -1.0f );
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

		dsq = (pdx*pdx + pdy*pdy + pdz*pdz) - dot*dot/lengthsq;

		if( dsq > radius_sq )
		{
			return( -1.0f );
		}
		else
		{
			return( dsq );		// return distance squared to axis
		}
	}
}
