#include <time.h>

#include <algorithm>
#include "libgcm/Math.hpp"
#include "libgcm/node/CalcNode.hpp"

#include <gtest/gtest.h>

using namespace gcm;

#define ITERATIONS 1000

#define MIN_ROOT 0.1
#define MAX_ROOT 1000

bool solveThirdOrderTest(real a1, real a2, real a3)
{
    real e[3];
    e[0] = a1;
    e[1] = a2;
    e[2] = a3;
    
    real a = - (e[0] + e[1] + e[2]);
    real b = e[0]*e[1] + e[0]*e[2] + e[1]*e[2];
    real c = - e[0] * e[1] * e[2];
    
    real r[3];
    solvePolynomialThirdOrder(a, b, c, r[0], r[1], r[2]);
    
    std::sort(std::begin(e), std::end(e));
    std::sort(std::begin(r), std::end(r));
    bool match = true;
    for( int i = 0; i < 3; i++ )
        if( fabs(e[i] - r[i]) > 0.025 * fabs(e[i] + r[i]) || isnan(r[i]) )
            match = false;
    return match;
};

TEST(Math, solvePolynomialThirdOrderThreeRoots)
{
    srand(0);

    real e[3];
    
    for (int k = 0; k < ITERATIONS; k++)
    {
        e[0] = MIN_ROOT + (MAX_ROOT-MIN_ROOT) * (double) rand() / RAND_MAX;
        e[1] = MIN_ROOT + (MAX_ROOT-MIN_ROOT) * (double) rand() / RAND_MAX;
        e[2] = MIN_ROOT + (MAX_ROOT-MIN_ROOT) * (double) rand() / RAND_MAX;
        
        ASSERT_TRUE( solveThirdOrderTest(e[0], e[1], e[2]) );
    }
};

TEST(Math, solvePolynomialThirdOrderTwoRoots)
{
    srand(0);

    real e[3];
    
    for (int k = 0; k < ITERATIONS; k++)
    {
        e[0] = MIN_ROOT + (MAX_ROOT-MIN_ROOT) * (double) rand() / RAND_MAX;
        e[1] = MIN_ROOT + (MAX_ROOT-MIN_ROOT) * (double) rand() / RAND_MAX;
        e[2] = e[1];
        
        ASSERT_TRUE( solveThirdOrderTest(e[0], e[1], e[2]) );
    }
}

TEST(Math, solvePolynomialThirdOrderOneRoot)
{
    srand(0);

    real e[3];
    
    for (int k = 0; k < ITERATIONS; k++)
    {
        e[0] = MIN_ROOT + (MAX_ROOT-MIN_ROOT) * (double) rand() / RAND_MAX;
        e[1] = e[0];
        e[2] = e[0];
        
        ASSERT_TRUE( solveThirdOrderTest(e[0], e[1], e[2]) );
    }
};

TEST(Math, solvePolynomialThirdOrderHydrostaticStress)
{
    srand(0);

    CalcNode node;
    
    for (int k = 0; k < ITERATIONS; k++)
    {
        real p = MIN_ROOT + (MAX_ROOT-MIN_ROOT) * (double) rand() / RAND_MAX;
        node.sxx = node.syy = node.szz = p;
        node.sxy = node.sxz = node.syz = 0;
        
        real a = - node.getJ1();
        real b = node.getJ2();
        real c = - node.getJ3();
        
        real r[3];
        solvePolynomialThirdOrder(a, b, c, r[0], r[1], r[2]);

        bool match = true;
        for( int i = 0; i < 3; i++ )
            if( fabs(p - r[i]) > 0.025 * fabs(p + r[i]) || isnan(r[i]) )
                match = false;
        
        ASSERT_TRUE( match );
    }
};

TEST(Math, vectorAdd)
{
    vector3r v1 = { 1, 2, 3 };
    vector3r v2 = { 3, 2, 1 };

    vector3r v = v1 + v2;

    ASSERT_FLOAT_EQ(v.x, 4);
    ASSERT_FLOAT_EQ(v.y, 4);
    ASSERT_FLOAT_EQ(v.z, 4);
}

TEST(Math, vectorSubtract)
{
    vector3r v1 = { 1, 2, 3 };
    vector3r v2 = { 3, 2, 1 };

    vector3r v = v1 - v2;

    ASSERT_FLOAT_EQ(v.x, -2);
    ASSERT_FLOAT_EQ(v.y, 0);
    ASSERT_FLOAT_EQ(v.z, 2);
}

TEST(Math, vectorScalarProduct)
{
    vector3r v1 = {1, 2, 3};
    vector3r v2 = {3, 2, 1};

    vector3r v = v1 - v2;

    ASSERT_FLOAT_EQ(v.x, -2);
    ASSERT_FLOAT_EQ(v.y, 0);
    ASSERT_FLOAT_EQ(v.z, 2);
}

TEST(Math, interpolateRectangle)
{
    real x1 = 0;
    real x2 = 5;
    real y1 = 0;
    real y2 = 3;

    real q11 = x1;
    real q12 = x1;
    real q21 = x2;
    real q22 = x2;

    real r;

    for (real x = x1; x < x2; x += 0.1)
        for (real y = y1; y < y2; y += 0.1)
        {
            interpolateRectangle(x1, y1, x2, y2, x, y, &q11, &q12, &q21, &q22, &r, 1);
            ASSERT_FLOAT_EQ(r, x);
        }

    q11 = y1;
    q12 = y2;
    q21 = y1;
    q22 = y2;

    for (real x = x1; x <= x2; x += 0.1)
        for (real y = y1; y <= y2; y += 0.1)
        {
            interpolateRectangle(x1, y1, x2, y2, x, y, &q11, &q12, &q21, &q22, &r, 1);
            ASSERT_FLOAT_EQ(r, y);
        }

}

TEST(Math, interpolateBox)
{
    real x0 = 0;
    real x1 = 3;
    real y0 = 0;
    real y1 = 5;
    real z0 = 0;
    real z1 = 7;


    real q000 = 1;
    real q010 = 2;
    real q110 = 3;
    real q100 = 4;

    real q001 = q000;
    real q011 = q010;
    real q111 = q110;
    real q101 = q100;

    real r1, r2;

    for (real x = x0; x <= x1; x += 0.1)
        for (real y = y0; y <= y1; y += 0.1)
            for (real z = z0; z <= z1; z += 0.1)
            {
                interpolateBox(x0, y0, z0, x1, y1, z1, x, y, z, &q000, &q001, &q010, &q011, &q100, &q101, &q110, &q111, &r1, 1);
                interpolateRectangle(x0, y0, x1, y1, x, y, &q000, &q010, &q100, &q110, &r2, 1);
                ASSERT_FLOAT_EQ(r1, r2);
            }

}
