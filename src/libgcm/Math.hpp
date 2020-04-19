/*
 * File:   Math.h
 * Author: anganar
 *
 * Created on May 3, 2013, 10:54 AM
 */

#ifndef GCM_MATH_H
#define    GCM_MATH_H

#include <string>
#include <string.h>
#include <cmath>
#include <limits>

#include "libgcm/util/Types.hpp"
#include "libgcm/Exception.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/util/Assertion.hpp"
#include "libgcm/config.hpp"

#define EQUALITY_TOLERANCE 0.00001


#if CONFIG_ENABLE_LOGGING
extern USE_LOGGER;
#endif

// Just scalar product of two vectors
float scalarProduct(float* a, float* b, float n);

float scalarProduct(float* a, float* b);

float scalarProduct(float x1, float y1, float z1, float x2, float y2, float z2);

float distance(const float* a, const float* b);

// Just vector product of two vectors
void vectorProduct(float x1, float y1, float z1, float x2, float y2, float z2, float *nx, float *ny, float *nz);

// Norm of the vector
float vectorNorm(float x1, float y1, float z1);

// Square of the norm of the vector
float vectorSquareNorm(float x1, float y1, float z1);

// Just determinant
float determinant(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);

// Finds volume of tetrahedron created on three given vectors
float tetrVolume(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);

// Finds area of triangle created on two given vectors
float triArea(float x1, float y1, float z1, float x2, float y2, float z2);

float tetrHeight(float coordsP0[3], float coordsP1[3], float coordsP2[3], float coordsP3[3]);

// Solid angle
float solidAngle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);

bool sameOrientation(float* base1, float* base2, float* check1, float* check2);

void findTriangleFaceNormal(const float *p1, const float *p2, const float *p3, float *x, float *y, float *z);

void interpolateTriangle(float *p1, float *p2, float *p3, float *p, float *v1, float *v2, float *v3, float *v, int n);

// checks if vector from p0 in direction v with lenght l intersects triangle p1-p2-p3
// if yes - point of intersection will be returned in p
// For algo - see http://www.cs.princeton.edu/courses/archive/fall00/cs426/lectures/raycast/sld016.htm and use the brain
bool vectorIntersectsTriangle(float *p1, float *p2, float *p3, float *p0, float *v, float l, float *p, bool debug);

bool pointInTriangle(float x, float y, float z, float coordsP0[3], float coordsP1[3], float coordsP2[3], bool debug);

bool pointInTetr(float x, float y, float z, float coordsP0[3], float coordsP1[3], float coordsP2[3], float coordsP3[3], bool debug);

// Create local basis based on the first vector - used to create the basis having normal only
void createLocalBasis(float n[], float n1[], float n2[]);

void shiftArrayLeft( int* arr, int n );

/*
 * P(x) = x^3 + a*x^2 + b*x + c
 * All the roots are real
 * See http://ru.wikipedia.org/wiki/Тригонометрическая_формула_Виета for algo
 */
void solvePolynomialThirdOrder(gcm::real a, gcm::real b, gcm::real c, gcm::real& root1, gcm::real& root2, gcm::real& root3);

inline int delta(int i, int j)
{
    return 1 ? i == j : 0;	
};

template <typename T>
int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}


/*
 * Bilinear interpolation in rectangle. See http://en.wikipedia.org/wiki/Bilinear_interpolation.
 */
void interpolateRectangle(gcm::real x1, gcm::real y1, gcm::real x2, gcm::real y2, gcm::real x, gcm::real y, const gcm::real* q11, const gcm::real* q12, const gcm::real* q21, const gcm::real* q22, gcm::real* out, uint n);

/*
 * Trilinear interpolation in cube. See http://en.wikipedia.org/wiki/Trilinear_interpolation.
 */
void interpolateBox(gcm::real x0, gcm::real y0, gcm::real z0, gcm::real x1, gcm::real y1, gcm::real z1, gcm::real x, gcm::real y, gcm::real z, const gcm::real* q000, const gcm::real* q001, const gcm::real* q010, const gcm::real* q011, const gcm::real* q100, const gcm::real* q101, const gcm::real* q110, const gcm::real* q111, gcm::real* out, uint n);


/**
 * Generate pseudorandom permutation of specified length
 * @param length required length
 * @return pointer to array contains permutation
 */
int* generateRandomPermutation(const int length);

#endif    /* GCM_MATH_H */
