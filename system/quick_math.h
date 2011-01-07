#ifndef _GCM_QUICK_MATH_H
#define _GCM_QUICK_MATH_H  1

#include <cmath>

class quick_math
{
public:
	float tetr_volume(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);
	float tri_area(float x1, float y1, float z1, float x2, float y2, float z2);
	float determinant(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);
};

// Finds volume of tetrahedron created on three given vectors
float quick_math::tetr_volume(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
	return determinant(x1, y1, z1, x2, y2, z2, x3, y3, z3)/6;
};

// Finds area of triangle created on two given vectors
float quick_math::tri_area(float x1, float y1, float z1, float x2, float y2, float z2)
{
	float a2 = x1*x1 + y1*y1 + z1*z1;
	float b2 = x2*x2 + y2*y2 + z2*z2;
	float ab = x1*x2 + y1*y2 + z1*z2;
	return sqrt(a2*b2 - ab*ab)/2;
};

// Just determinant
float quick_math::determinant(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
	return (x1*(y2*z3-y3*z2) - x2*(y1*z3-y3*z1) + x3*(y1*z2-y2*z1));
};

#endif
