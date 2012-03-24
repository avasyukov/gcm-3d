#ifndef _GCM_QUICK_MATH_H
#define _GCM_QUICK_MATH_H  1

#include <cmath>

class quick_math
{
public:
	float tetr_volume(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);
	float tri_area(float x1, float y1, float z1, float x2, float y2, float z2);
	float determinant(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);
	float scalar_product(float x1, float y1, float z1, float x2, float y2, float z2);
	void vector_product(float x1, float y1, float z1, float x2, float y2, float z2, float *nx, float *ny, float *nz);
	bool same_orientation(float* base1, float* base2, float* check1, float* check2);
	float vector_norm(float x1, float y1, float z1);
	float solid_angle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);
	static const float PI;
};

#endif
