#include <gtest/gtest.h>

#include "libgcm/Math.hpp"


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
