#include <time.h>

#include <algorithm>
#include "libgcm/Math.hpp"

#include <gtest/gtest.h>

using namespace gcm;

#define ITERATIONS 1000

#define MIN_ROOT 0.1
#define MAX_ROOT 1.0

bool solveThirdOrderTest(gcm_real a1, gcm_real a2, gcm_real a3)
{
    gcm_real e[3];
    e[0] = a1;
    e[1] = a2;
    e[2] = a3;
    
    gcm_real a = - (e[0] + e[1] + e[2]);
    gcm_real b = e[0]*e[1] + e[0]*e[2] + e[1]*e[2];
    gcm_real c = - e[0] * e[1] * e[2];
    
    gcm_real r[3];
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

    gcm_real e[3];
    
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

    gcm_real e[3];
    
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

    gcm_real e[3];
    
    for (int k = 0; k < ITERATIONS; k++)
    {
        e[0] = MIN_ROOT + (MAX_ROOT-MIN_ROOT) * (double) rand() / RAND_MAX;
        e[1] = e[0];
        e[2] = e[0];
        
        ASSERT_TRUE( solveThirdOrderTest(e[0], e[1], e[2]) );
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

TEST(Math, isPointInNormalDirection)
{
    ASSERT_TRUE(isPointInNormalDirection({0, 0, 0}, {0, 0, 1}, {0, 0, 0.5}));
    ASSERT_FALSE(isPointInNormalDirection({0, 0, 0}, {0, 0, 1}, {0, 0, -0.5}));
    
    ASSERT_TRUE(isPointInNormalDirection({1, 0, 0}, {1, 1, 1}, {3, 2, 1}));
    ASSERT_FALSE(isPointInNormalDirection({1, 0, 0}, {1, 1, 1}, {-3, -2, -1}));
    
    ASSERT_TRUE(isPointInNormalDirection({1, 1, 1}, {0, 0, 1}, {1, 1, 1}));
}
