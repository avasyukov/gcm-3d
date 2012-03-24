#include "quick_math.h"

// is M_PI soooo bad? :)
const float quick_math::PI = atanf(1) * 4;

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

// Just scalar product of two vectors
float quick_math::scalar_product(float x1, float y1, float z1, float x2, float y2, float z2)
{
	return x1*x2 + y1*y2 + z1*z2;
};

void quick_math::vector_product(float x1, float y1, float z1, float x2, float y2, float z2, float *nx, float *ny, float *nz)
{
	*nx = y1 * z2 - y2 * z1;
	*ny = x2 * z1 - x1 * z2;
	*nz = x1 * y2 - x2 * y1;
};

bool quick_math::same_orientation(float* base1, float* base2, float* check1, float* check2)
{
	// tmp normals used to check tri orientation
	float n1[3];
	float n2[3];

	vector_product( 	base1[0] - check1[0], 	base1[1] - check1[1], 	base1[2] - check1[2],
				base2[0] - check1[0], 	base2[1] - check1[1], 	base2[2] - check1[2],
				&n1[0], 		&n1[1], 		&n1[2] 			);

	vector_product( 	base1[0] - check2[0], 	base1[1] - check2[1], 	base1[2] - check2[2],
				base2[0] - check2[0], 	base2[1] - check2[1], 	base2[2] - check2[2],
				&n2[0], 		&n2[1], 		&n2[2]	 		);

	if( scalar_product(n1[0], n1[1], n1[2], n2[0], n2[1], n2[2]) < 0)
		return false;
	return true;
};

// Norm of the vector
float quick_math::vector_norm(float x1, float y1, float z1)
{
	return sqrt( scalar_product(x1, y1, z1, x1, y1, z1) );
};

// Solid angle 
float quick_math::solid_angle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
	float det = fabs( determinant(x1, y1, z1, x2, y2, z2, x3, y3, z3) );
	float norm_v1 = vector_norm(x1, y1, z1);
	float norm_v2 = vector_norm(x2, y2, z2);
	float norm_v3 = vector_norm(x3, y3, z3);
	float div = norm_v1 * norm_v2 * norm_v3 + scalar_product(x1, y1, z1, x2, y2, z2) * norm_v3 
					+ scalar_product(x1, y1, z1, x3, y3, z3) * norm_v2 + scalar_product(x2, y2, z2, x3, y3, z3) * norm_v1;
	float at = atanf( det / div );
	if (at < 0) { at += PI; }

	return 2 * at;
};
