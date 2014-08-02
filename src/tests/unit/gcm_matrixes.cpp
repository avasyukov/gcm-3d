#include <time.h>

#include "libgcm/util/matrixes.hpp"

#include <gtest/gtest.h>

using namespace gcm;

TEST(GcmMatrixes, Equality)
{
    srand(time(NULL));

    gcm_matrix a;
    gcm_matrix b;
    
    for( int i = 0; i < GCM_MATRIX_SIZE; i++ )
    {
        for( int j = 0; j < GCM_MATRIX_SIZE; j++ )
        {
            a(i,j) = 1.0 * (double) rand() / RAND_MAX;
            b(i,j) = a(i,j);
        }
    }
    
    b(1,1) = a(1,1) * 1.01;
    ASSERT_TRUE( a == b );
    
    b(1,1) = a(1,1) * 1.02;
    ASSERT_FALSE( a == b );
};

TEST(GcmMatrixes, BasicMath)
{
    srand(time(NULL));

    gcm_matrix a;
    gcm_matrix b;
    gcm_matrix e;
    gcm_matrix z;
    
    for( int i = 0; i < GCM_MATRIX_SIZE; i++ )
    {
        for( int j = 0; j < GCM_MATRIX_SIZE; j++ )
        {
            a(i,j) = 1.0 * (double) rand() / RAND_MAX;
        }
    }
    
    z.clear();
    
    ASSERT_TRUE( a - a == z);
    ASSERT_TRUE( a * 0 == z );
    ASSERT_TRUE( a + a == a * 2 );
    ASSERT_TRUE( a - a * 0.5 == a / 2 );
    ASSERT_ANY_THROW( a / 0 );
    
    b = a.inv();
    e.createE();
    
    ASSERT_TRUE( a * b == e );
    ASSERT_TRUE( e / a == b );
};