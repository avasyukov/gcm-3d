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
//#include "libgcm/Math.hpp"
#include "libgcm/Exception.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/util/Assertion.hpp"
#include "libgcm/config.hpp"

#define EQUALITY_TOLERANCE 0.00001

using namespace std;
using namespace gcm;

#if CONFIG_ENABLE_LOGGING
extern USE_LOGGER;
#endif

// Just scalar product of two vectors
inline float scalarProduct(float* a, float* b, float n)
{
    float result = 0;
    for( int i = 0; i < n; i++ )
        result += a[i]*b[i];
    return result;
};

inline float scalarProduct(float* a, float* b)
{
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
};

inline float scalarProduct(float x1, float y1, float z1, float x2, float y2, float z2)
{
    return x1*x2 + y1*y2 + z1*z2;
};

inline float distance(float* a, float* b)
{
    return sqrt( (a[0] - b[0])*(a[0] - b[0]) + (a[1] - b[1])*(a[1] - b[1]) +(a[2] - b[2])*(a[2] - b[2]) );
};

// Just vector product of two vectors
inline void vectorProduct(float x1, float y1, float z1, float x2, float y2, float z2, float *nx, float *ny, float *nz)
{
    *nx = y1 * z2 - y2 * z1;
    *ny = x2 * z1 - x1 * z2;
    *nz = x1 * y2 - x2 * y1;
};

// Norm of the vector
inline float vectorNorm(float x1, float y1, float z1)
{
    return sqrt( scalarProduct(x1, y1, z1, x1, y1, z1) );
};

// Square of the norm of the vector
inline float vectorSquareNorm(float x1, float y1, float z1)
{
    return scalarProduct(x1, y1, z1, x1, y1, z1);
};

// Just determinant
inline float determinant(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
    return (x1*(y2*z3-y3*z2) - x2*(y1*z3-y3*z1) + x3*(y1*z2-y2*z1));
};

// Finds volume of tetrahedron created on three given vectors
inline float tetrVolume(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
    return determinant(x1, y1, z1, x2, y2, z2, x3, y3, z3)/6;
};

// Finds area of triangle created on two given vectors
inline float triArea(float x1, float y1, float z1, float x2, float y2, float z2)
{
    float a2 = x1*x1 + y1*y1 + z1*z1;
    float b2 = x2*x2 + y2*y2 + z2*z2;
    float ab = x1*x2 + y1*y2 + z1*z2;
    float diff = a2*b2 - ab*ab;
    // It happens sometimes for zero-area triangles because of floating point rounding errors
    if(diff < 0)
        return 0;
    return sqrt(diff)/2;
};

inline float tetrHeight(float coordsP0[3], float coordsP1[3], float coordsP2[3], float coordsP3[3])
{
        float area[4];
        float maxArea;

        // Find volume
        float vol = fabs( tetrVolume(
            coordsP1[0] - coordsP0[0], coordsP1[1] - coordsP0[1], coordsP1[2] - coordsP0[2],
            coordsP2[0] - coordsP0[0], coordsP2[1] - coordsP0[1], coordsP2[2] - coordsP0[2],
            coordsP3[0] - coordsP0[0], coordsP3[1] - coordsP0[1], coordsP3[2] - coordsP0[2]
        ) );

        // Find area of first face (verticles - 0,1,2)
        area[0] = triArea(
            coordsP1[0] - coordsP0[0], coordsP1[1] - coordsP0[1], coordsP1[2] - coordsP0[2],
            coordsP2[0] - coordsP0[0], coordsP2[1] - coordsP0[1], coordsP2[2] - coordsP0[2]
        );

        // Find area of second face (verticles - 0,1,3)
        area[1] = triArea(
            coordsP1[0] - coordsP0[0], coordsP1[1] - coordsP0[1], coordsP1[2] - coordsP0[2],
            coordsP3[0] - coordsP0[0], coordsP3[1] - coordsP0[1], coordsP3[2] - coordsP0[2]
        );

        // Find area of third face (verticles - 0,2,3)
        area[2] = triArea(
            coordsP2[0] - coordsP0[0], coordsP2[1] - coordsP0[1], coordsP2[2] - coordsP0[2],
            coordsP3[0] - coordsP0[0], coordsP3[1] - coordsP0[1], coordsP3[2] - coordsP0[2]
        );

        // Find area of third face (verticles - 1,2,3)
        area[3] = triArea(
            coordsP2[0] - coordsP1[0], coordsP2[1] - coordsP1[1], coordsP2[2] - coordsP1[2],
            coordsP3[0] - coordsP1[0], coordsP3[1] - coordsP1[1], coordsP3[2] - coordsP1[2]
        );

        // Check if all nodes are already loaded from other CPUs and tetrahadron is correct
        assert_gt(vol, 0);
        for(int j = 0; j < 4; j++)
            assert_gt(area[j], 0);

        // Find maximum face area
        maxArea = area[0];
        for(int j = 1; j < 4; j++)
            if( area[j] > maxArea )
                maxArea = area[j];

        // Return minumal height of this tetrahedron
        return fabs( 3*vol / maxArea );
};

// Solid angle
inline float solidAngle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
    float det = fabs( determinant(x1, y1, z1, x2, y2, z2, x3, y3, z3) );
    float norm_v1 = vectorNorm(x1, y1, z1);
    float norm_v2 = vectorNorm(x2, y2, z2);
    float norm_v3 = vectorNorm(x3, y3, z3);
    float div = norm_v1 * norm_v2 * norm_v3
                    + scalarProduct(x1, y1, z1, x2, y2, z2) * norm_v3
                    + scalarProduct(x1, y1, z1, x3, y3, z3) * norm_v2
                    + scalarProduct(x2, y2, z2, x3, y3, z3) * norm_v1;
    float at = atanf( det / div );
    if (at < 0)
        at += M_PI;

    return 2 * at;
};

inline bool sameOrientation(float* base1, float* base2, float* check1, float* check2)
{
    // tmp normals used to check tri orientation
    float n1[3];
    float n2[3];

    vectorProduct(
            base1[0] - check1[0],     base1[1] - check1[1],     base1[2] - check1[2],
            base2[0] - check1[0],     base2[1] - check1[1],     base2[2] - check1[2],
            &n1[0],                    &n1[1],                    &n1[2]
    );

    vectorProduct(
            base1[0] - check2[0],     base1[1] - check2[1],     base1[2] - check2[2],
            base2[0] - check2[0],     base2[1] - check2[1],     base2[2] - check2[2],
            &n2[0],                    &n2[1],                    &n2[2]
    );

    if( scalarProduct(n1[0], n1[1], n1[2], n2[0], n2[1], n2[2]) < 0)
        return false;

    return true;
};

inline void findTriangleFaceNormal(const float *p1, const float *p2, const float *p3, float *x, float *y, float *z)
{
    // Normal vector
    float normal[3];

    // Tmp length
    float scale;

    vectorProduct(
            p2[0] - p1[0],     p2[1] - p1[1],     p2[2] - p1[2],
            p3[0] - p1[0],     p3[1] - p1[1],     p3[2] - p1[2],
            &normal[0],        &normal[1],        &normal[2]
    );

    // Normal length
    scale = 1 / vectorNorm( normal[0], normal[1], normal[2] );

    // Return normalized normal vector
    *x = normal[0] * scale;
    *y = normal[1] * scale;
    *z = normal[2] * scale;
}

inline void interpolateTriangle(float *p1, float *p2, float *p3, float *p, float *v1, float *v2, float *v3, float *v, int n)
{
    float areas[3];
    areas[0] = fabs( triArea(p3[0] - p[0], p3[1] - p[1], p3[2] - p[2], p2[0] - p[0], p2[1] - p[1], p2[2] - p[2]) );
    areas[1] = fabs( triArea(p1[0] - p[0], p1[1] - p[1], p1[2] - p[2], p2[0] - p[0], p2[1] - p[1], p2[2] - p[2]) );
    areas[2] = fabs( triArea(p3[0] - p[0], p3[1] - p[1], p3[2] - p[2], p1[0] - p[0], p1[1] - p[1], p1[2] - p[2]) );

    float l = areas[0] + areas[1] + areas[2];
    float _l1 = areas[0] / l;
    float _l2 = areas[2] / l;
    float _l3 = areas[1] / l;

    // interpolate
    for(int i = 0; i < n; i++)
        v[i] = _l1*v1[i] + _l2*v2[i] + _l3*v3[i];
}

// checks if vector from p0 in direction v with lenght l intersects triangle p1-p2-p3
// if yes - point of intersection will be returned in p
// For algo - see http://www.cs.princeton.edu/courses/archive/fall00/cs426/lectures/raycast/sld016.htm and use the brain
inline bool vectorIntersectsTriangle(float *p1, float *p2, float *p3, float *p0,
        float *v, float l, float *p, bool debug)
{
    if( debug )
    {
        LOG_TRACE("Point: " << p0[0] + l*v[0] << " " << p0[1] + l*v[1] << " " << p0[2] + l*v[2]);
        LOG_TRACE("Verticle #1: " << p1[0] << " " << p1[1] << " " << p1[2]);
        LOG_TRACE("Verticle #2: " << p2[0] << " " << p2[1] << " " << p2[2]);
        LOG_TRACE("Verticle #3: " << p3[0] << " " << p3[1] << " " << p3[2]);
    }

    bool result = true;
    // p is point of intersection

    // face normal
    float n[3];
    // face plane parameter
    float d;
    // distance
    float t;

    // Get face normal
    findTriangleFaceNormal(p1, p2, p3, &n[0], &n[1], &n[2]);

    // If vector is parallel to face - no intersection
    float vn = scalarProduct(n[0], n[1], n[2], v[0], v[1], v[2]);
    if (vn * vn < EQUALITY_TOLERANCE * vectorSquareNorm (n[0], n[1], n[2]) * vectorSquareNorm (v[0], v[1], v[2]))
        result = false;

    // find plane parameter
    d = - scalarProduct(n[0], n[1], n[2], p1[0], p1[1], p1[2]);

    // find distance to the plane
    t = - (scalarProduct(n[0], n[1], n[2], p0[0], p0[1], p0[2]) + d) / vn;

    if( debug )
    {
        LOG_TRACE("Parameter: t = " << t);
    }

    // If distance is too big - no intersection
    // If we need opposite direction - no intersection as well
    if( (t < -EQUALITY_TOLERANCE) || (t > l+EQUALITY_TOLERANCE) )
        result = false;

    // find point of intersection with the plane
    for(int i = 0; i < 3; i++)
        p[i] = p0[i] + t * v[i];

    if( debug )
    {
        LOG_TRACE("Intersection: " << p[0] << " " << p[1] << " "<< p[2]);
    }

    /*
     * We do not use this impl now since it looks affected by floating point rounding issues
     *
    // check that point is inside triangle
    if( ! sameOrientation(p1, p2, p3, p) )
        result = false;
    if( ! sameOrientation(p1, p3, p2, p) )
        result = false;
    if( ! sameOrientation(p2, p3, p1, p) )
        result = false;
     */
    // Small workaround
    if( !result )
        return false;

    // According with original algo we have all tests passed.
    // So, it should really intersect.
    // However, we can have floating point rounding issues.
    // So, continue checking.

    bool resultArea = true;
    float area = fabs( triArea( p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2],
                                p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2]) );
    float areas[3];
    areas[0] = fabs( triArea( p3[0] - p[0], p3[1] - p[1], p3[2] - p[2],
                                p2[0] - p[0], p2[1] - p[1], p2[2] - p[2]) );
    areas[1] = fabs( triArea(p1[0] - p[0], p1[1] - p[1], p1[2] - p[2],
                                p2[0] - p[0], p2[1] - p[1], p2[2] - p[2]) );
    areas[2] = fabs( triArea(p3[0] - p[0], p3[1] - p[1], p3[2] - p[2],
                                p1[0] - p[0], p1[1] - p[1], p1[2] - p[2]) );

    if( fabs(areas[0] + areas[1] + areas[2] - area) > area * 1000 * EQUALITY_TOLERANCE )
        resultArea = false;

    if( debug )
    {
        LOG_TRACE("Areas: " << area << " " << areas[0] << " " << areas[1] << " " << areas[2]);
        LOG_TRACE("Factor: " << (areas[0] + areas[1] + areas[2]) / area);
    }

    // assert_ne(result, resultArea);

    return resultArea;

//    *logger << "P: " << p[0] << " " << p[1] << " " < p[2];
/*    if(res1 != result) {
        *logger << "Orient res: " << result << " Area res: " << res1 << " with " < (areas[0] + areas[1] + areas[2] > area * 1.00001);
        *logger << "Orients: " << qm_engine.same_orientation(p1, p2, p3, p) << " "
                << qm_engine.same_orientation(p1, p3, p2, p) << " "
                < qm_engine.same_orientation(p2, p3, p1, p);
        *logger << "Area: " << area << " Areas: " << areas[0] << " " << areas[1] << " " << areas[2] << " Sum: " < (areas[0] + areas[1] + areas[2]);
        *logger << "P0: " << p1[0] << " " << p1[1] << " " < p1[2];
        *logger << "P1: " << p1[0] << " " << p1[1] << " " < p1[2];
        *logger << "P2: " << p2[0] << " " << p2[1] << " " < p2[2];
        *logger << "P3: " << p3[0] << " " << p3[1] << " " < p3[2];
        *logger << "P: " << p[0] << " " << p[1] << " " < p[2];
//        throw GCMException( GCMException::MESH_EXCEPTION, "Different results for vector_intersects_triangle");
    }*/

    // We need this fix to avoid issues with crosses like (0.0990249142 2.1982545853 11095970816.0000000000).
    // We get these issues for secong order nodes when using second order mesh.
    //if( res1 &&
    //    ( ( p[0] < outline.min_coords[0] ) || ( p[0] > outline.max_coords[0] )
    //        || ( p[1] < outline.min_coords[1] ) || ( p[1] > outline.max_coords[1] )
    //        || ( p[2] < outline.min_coords[2] ) || ( p[2] > outline.max_coords[2] ) ) )
    //{
        /**logger << "P0: " << p0[0] << " " << p0[1] << " " < p0[2];
        *logger << "P1: " << p1[0] << " " << p1[1] << " " < p1[2];
        *logger << "P2: " << p2[0] << " " << p2[1] << " " < p2[2];
        *logger << "P3: " << p3[0] << " " << p3[1] << " " < p3[2];
        *logger << "P: " << p[0] << " " << p[1] << " " < p[2];
        *logger << "V: " << v[0] << " " << v[1] << " " < v[2];
        *logger << "L: " < l;*/
        //throw GCMException( GCMException::MESH_EXCEPTION, "Error in finding border cross");
    //    res1 = false;
    //}
};

inline bool pointInTriangle(float x, float y, float z,
        float coordsP0[3], float coordsP1[3], float coordsP2[3], bool debug)
{

    float v0[3];
    float v1[3];
    float v2[3];
    for( int i = 0; i < 3; i++ )
    {
        v0[i] = coordsP2[i] - coordsP0[i];
        v1[i] = coordsP1[i] - coordsP0[i];
    }
    v2[0] = x - coordsP0[0];
    v2[1] = y - coordsP0[1];
    v2[2] = z - coordsP0[2];

    float l = (fabs (v0[0]) + fabs (v0[1]) + fabs (v0[2])
            + fabs (v1[0]) + fabs (v1[1]) + fabs (v1[2])
            + fabs (v2[0]) + fabs (v2[1]) + fabs (v2[2])) / 9;
    // Check if point is in plane defined by triangle
    // If not - just return false
    if( fabs( tetrVolume( v0[0], v0[1], v0[2],
                          v1[0], v1[1], v1[2],
                          v2[0], v2[1], v2[2]) ) > EQUALITY_TOLERANCE * l * l * l )
        return false;

    // Otherwise use this algorithm - http://www.blackpawn.com/texts/pointinpoly/default.html

    // Compute barycentric coordinates
    float dot00 = scalarProduct(v0[0], v0[1], v0[2], v0[0], v0[1], v0[2]);
    float dot01 = scalarProduct(v0[0], v0[1], v0[2], v1[0], v1[1], v1[2]);
    float dot02 = scalarProduct(v0[0], v0[1], v0[2], v2[0], v2[1], v2[2]);
    float dot11 = scalarProduct(v1[0], v1[1], v1[2], v1[0], v1[1], v1[2]);
    float dot12 = scalarProduct(v1[0], v1[1], v1[2], v2[0], v2[1], v2[2]);
    float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    if( debug )
    {
        LOG_TRACE("Point: " << x << " " << y << " " << z);
        LOG_TRACE("Verticle #1: " << coordsP0[0] << " " << coordsP0[1] << " " << coordsP0[2]);
        LOG_TRACE("Verticle #2: " << coordsP1[0] << " " << coordsP1[1] << " " << coordsP1[2]);
        LOG_TRACE("Verticle #3: " << coordsP2[0] << " " << coordsP2[1] << " " << coordsP2[2]);
        LOG_TRACE("u = " << u << "; v = " << v << "; u+v = " << u + v);
    }

    return (u >= -EQUALITY_TOLERANCE) && (v >= -EQUALITY_TOLERANCE) && (u + v <= 1 + EQUALITY_TOLERANCE);
};

inline bool pointInTetr(float x, float y, float z,
        float coordsP0[3], float coordsP1[3], float coordsP2[3], float coordsP3[3], bool debug)
{
    if( debug )
    {
        LOG_TRACE("Point: " << x << " " << y << " " << z);
        LOG_TRACE("Verticle #1: " << coordsP0[0] << " " << coordsP0[1] << " " << coordsP0[2]);
        LOG_TRACE("Verticle #2: " << coordsP1[0] << " " << coordsP1[1] << " " << coordsP1[2]);
        LOG_TRACE("Verticle #3: " << coordsP2[0] << " " << coordsP2[1] << " " << coordsP2[2]);
        LOG_TRACE("Verticle #4: " << coordsP3[0] << " " << coordsP3[1] << " " << coordsP3[2]);
    }

    float vol;
    float vols[4];

    vol = fabs( determinant(
        coordsP1[0] - coordsP0[0], coordsP1[1] - coordsP0[1], coordsP1[2] - coordsP0[2],
        coordsP2[0] - coordsP0[0], coordsP2[1] - coordsP0[1], coordsP2[2] - coordsP0[2],
        coordsP3[0] - coordsP0[0], coordsP3[1] - coordsP0[1], coordsP3[2] - coordsP0[2]
    ) );

    vols[0] = fabs( determinant(
        coordsP1[0] - x, coordsP1[1] - y, coordsP1[2] - z,
        coordsP2[0] - x, coordsP2[1] - y, coordsP2[2] - z,
        coordsP3[0] - x, coordsP3[1] - y, coordsP3[2] - z
    ) );

    vols[1] = fabs( determinant(
        coordsP0[0] - x, coordsP0[1] - y, coordsP0[2] - z,
        coordsP2[0] - x, coordsP2[1] - y, coordsP2[2] - z,
        coordsP3[0] - x, coordsP3[1] - y, coordsP3[2] - z
    ) );

    vols[2] = fabs( determinant(
        coordsP1[0] - x, coordsP1[1] - y, coordsP1[2] - z,
        coordsP0[0] - x, coordsP0[1] - y, coordsP0[2] - z,
        coordsP3[0] - x, coordsP3[1] - y, coordsP3[2] - z
    ) );

    vols[3] = fabs( determinant(
        coordsP1[0] - x, coordsP1[1] - y, coordsP1[2] - z,
        coordsP2[0] - x, coordsP2[1] - y, coordsP2[2] - z,
        coordsP0[0] - x, coordsP0[1] - y, coordsP0[2] - z
    ) );

    if( debug )
    {
        LOG_TRACE("Volumes: " << vol << " " << vols[0] << " " << vols[1] << " " << vols[2] << " " << vols[3]);
        LOG_TRACE("Factor: " << (vols[0] + vols[1] + vols[2] + vols[3]) / vol );
    }

    if( vols[0] + vols[1] + vols[2] + vols[3] < vol * (1 + 10 * EQUALITY_TOLERANCE) ) {
        if(debug) { LOG_TRACE("IN"); }
        return true;
    } else {
        if(debug) { LOG_TRACE("OUT"); }
        return false;
    }

    /*float d1,d2;
    d1 = determinant(
        coordsP1[0] - coordsP0[0], coordsP1[1] - coordsP0[1], coordsP1[2] - coordsP0[2],
        coordsP2[0] - coordsP0[0], coordsP2[1] - coordsP0[1], coordsP2[2] - coordsP0[2],
        coordsP3[0] - coordsP0[0], coordsP3[1] - coordsP0[1], coordsP3[2] - coordsP0[2]
    );
    d2 = determinant(
        coordsP1[0] - x, coordsP1[1] - y, coordsP1[2] - z,
        coordsP2[0] - x, coordsP2[1] - y, coordsP2[2] - z,
        coordsP3[0] - x, coordsP3[1] - y, coordsP3[2] - z
    );
    if( debug )
        LOG_TRACE("D1 = " << d1 << " D2 = " << d2 << " D1*D2 = " << d1*d2);
    if(d1*d2 < 0) { return false; }

    d1 = determinant(
        coordsP0[0] - coordsP1[0], coordsP0[1] - coordsP1[1], coordsP0[2] - coordsP1[2],
        coordsP2[0] - coordsP1[0], coordsP2[1] - coordsP1[1], coordsP2[2] - coordsP1[2],
        coordsP3[0] - coordsP1[0], coordsP3[1] - coordsP1[1], coordsP3[2] - coordsP1[2]
    );
    d2 = determinant(
        coordsP0[0] - x, coordsP0[1] - y, coordsP0[2] - z,
        coordsP2[0] - x, coordsP2[1] - y, coordsP2[2] - z,
        coordsP3[0] - x, coordsP3[1] - y, coordsP3[2] - z
    );
    if( debug )
        LOG_TRACE("D1 = " << d1 << " D2 = " << d2 << " D1*D2 = " << d1*d2);
    if(d1*d2 < 0) { return false; }

    d1 = determinant(
        coordsP0[0] - coordsP2[0], coordsP0[1] - coordsP2[1], coordsP0[2] - coordsP2[2],
        coordsP1[0] - coordsP2[0], coordsP1[1] - coordsP2[1], coordsP1[2] - coordsP2[2],
        coordsP3[0] - coordsP2[0], coordsP3[1] - coordsP2[1], coordsP3[2] - coordsP2[2]
    );
    d2 = determinant(
        coordsP0[0] - x, coordsP0[1] - y, coordsP0[2] - z,
        coordsP1[0] - x, coordsP1[1] - y, coordsP1[2] - z,
        coordsP3[0] - x, coordsP3[1] - y, coordsP3[2] - z
    );
    if( debug )
        LOG_TRACE("D1 = " << d1 << " D2 = " << d2 << " D1*D2 = " << d1*d2);
    if(d1*d2 < 0) { return false; }

    d1 = determinant(
        coordsP0[0] - coordsP3[0], coordsP0[1] - coordsP3[1], coordsP0[2] - coordsP3[2],
        coordsP1[0] - coordsP3[0], coordsP1[1] - coordsP3[1], coordsP1[2] - coordsP3[2],
        coordsP2[0] - coordsP3[0], coordsP2[1] - coordsP3[1], coordsP2[2] - coordsP3[2]
    );
    d2 = determinant(
        coordsP0[0] - x, coordsP0[1] - y, coordsP0[2] - z,
        coordsP1[0] - x, coordsP1[1] - y, coordsP1[2] - z,
        coordsP2[0] - x, coordsP2[1] - y, coordsP2[2] - z
    );
    if( debug )
        LOG_TRACE("D1 = " << d1 << " D2 = " << d2 << " D1*D2 = " << d1*d2);
    if(d1*d2 < 0) { return false; }

    return true;*/
};

// Create local basis based on the first vector - used to create the basis having normal only
inline void createLocalBasis(float n[], float n1[], float n2[])
{
    assert_lt(fabs( vectorNorm(n[0], n[1], n[2]) - 1 ), EQUALITY_TOLERANCE );

    if(fabs(n[0]) <= fabs(n[1])) {
        if(fabs(n[0]) <= fabs(n[2])) {
            n1[0] = 0;
            n1[1] = -n[2];
            n1[2] = n[1];
        } else {
            n1[0] = -n[1];
            n1[1] = n[0];
            n1[2] = 0;
        }
    } else {
        if(fabs(n[1]) <= fabs(n[2])) {
            n1[0] = -n[2];
            n1[1] = 0;
            n1[2] = n[0];
        } else {
            n1[0] = -n[1];
            n1[1] = n[0];
            n1[2] = 0;
        }
    }

    float dtmp = 1 / vectorNorm( n1[0], n1[1], n1[2] );
    n1[0] *= dtmp;
    n1[1] *= dtmp;
    n1[2] *= dtmp;

    vectorProduct(n[0], n[1], n[2], n1[0], n1[1], n1[2], &n2[0], &n2[1], &n2[2]);
};

inline void shiftArrayLeft( int* arr, int n )
{
    assert_gt(n, 0);
    int a0 = arr[0];
    memmove(arr, arr+1, (n-1)*sizeof(int));
    arr[n-1] = a0;
};

inline int delta(int i, int j)
{
	return 1 ? i == j : 0;	
};

template <typename T>
int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

#endif    /* GCM_MATH_H */
