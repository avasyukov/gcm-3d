#include "libgcm/linal/Matrix.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <iostream>

using namespace gcm;
using namespace gcm::linal;

template<uint M, uint N>
class MatrixWrapper: public Matrix<M, N>
{
    public:
        uint _getIndex(uint i, uint j) const
        {
            return this->getIndex(i, j);
        }
};

TEST(Linal, ContainerIndex)
{
    MatrixWrapper<2, 8> m1;
    MatrixWrapper<8, 2> m2;

    for (uint i = 0; i < 2; i++)
        for (uint j = 0; j < 8; j++)
        {
            ASSERT_LT(m1._getIndex(i, j), 16);
            ASSERT_LT(m2._getIndex(j, i), 16);
        }
}

TEST(Linal, MatrixConstruct)
{
    Matrix<3, 3> m1({
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0,
        7.0, 8.0, 9.0
    });

    ASSERT_EQ(m1.values[0], 1.0);
    ASSERT_EQ(m1.values[1], 2.0);
    ASSERT_EQ(m1.values[2], 3.0);
    ASSERT_EQ(m1.values[3], 4.0);
    ASSERT_EQ(m1.values[4], 5.0);
    ASSERT_EQ(m1.values[5], 6.0);
    ASSERT_EQ(m1.values[6], 7.0);
    ASSERT_EQ(m1.values[7], 8.0);
    ASSERT_EQ(m1.values[8], 9.0);


    Matrix<3, 3, Matrix33Container> m2({
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0,
        7.0, 8.0, 9.0
    });
    
    ASSERT_EQ(m2.xx, 1.0);
    ASSERT_EQ(m2.xy, 2.0);
    ASSERT_EQ(m2.xz, 3.0);
    ASSERT_EQ(m2.yx, 4.0);
    ASSERT_EQ(m2.yy, 5.0);
    ASSERT_EQ(m2.yz, 6.0);
    ASSERT_EQ(m2.zx, 7.0);
    ASSERT_EQ(m2.zy, 8.0);
    ASSERT_EQ(m2.zz, 9.0);
}

TEST(Linal, MatrixAccess)
{
    Matrix<3, 3> m({
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0,
        7.0, 8.0, 9.0
    });

    ASSERT_EQ(m(0, 0), 1.0);

    const auto& m_ref = m;

    ASSERT_EQ(m_ref(0, 0), 1.0);

    m(0, 0) = 10.0;
    ASSERT_EQ(m(0, 0), 10.0);
}


TEST(Linal, MatrixAdd)
{
    Matrix<2, 2> m1({
        1.0, 2.0,
        3.0, 4.0
    });
    
    Matrix<2, 2> m2({
        4.0, 3.0,
        2.0, 1.0
    });

    auto m3 = m1 + m2;

    ASSERT_EQ(m3(0, 0), 5.0);
    ASSERT_EQ(m3(0, 1), 5.0);
    ASSERT_EQ(m3(1, 0), 5.0);
    ASSERT_EQ(m3(1, 1), 5.0);
}


TEST(Linal, MatrixAddCustomContainer)
{
    Matrix<2, 2, Matrix22Container> m1({
        1.0, 2.0,
        3.0, 4.0
    });
    
    Matrix<2, 2, Matrix22Container> m2({
        4.0, 3.0,
        2.0, 1.0
    });

    auto m3 = m1 + m2;

    ASSERT_EQ(m3.xx, 5.0);
    ASSERT_EQ(m3.xy, 5.0);
    ASSERT_EQ(m3.yx, 5.0);
    ASSERT_EQ(m3.yy, 5.0);
}

TEST(Linal, MatrixAssign)
{
    Matrix<2, 2> m1({
        1.0, 2.0,
        3.0, 4.0
    });

    auto m2 = m1;

    ASSERT_EQ(m2(0, 0), 1.0);
    ASSERT_EQ(m2(0, 1), 2.0);
    ASSERT_EQ(m2(1, 0), 3.0);
    ASSERT_EQ(m2(1, 1), 4.0);
}

TEST(Linal, MatrixSubtract)
{
    Matrix<2, 2> m1({
        1.0, 2.0,
        3.0, 4.0
    });

    auto m2 = m1;
    

    auto m3 = m1 - m2;

    ASSERT_EQ(m3(0, 0), 0.0);
    ASSERT_EQ(m3(0, 1), 0.0);
    ASSERT_EQ(m3(1, 0), 0.0);
    ASSERT_EQ(m3(1, 1), 0.0);
}


TEST(Linal, MatrixSubtractCustomContainer)
{
    Matrix<2, 2, Matrix22Container> m1({
        1.0, 2.0,
        3.0, 4.0
    });

    auto m2 = m1;

    auto m3 = m1 - m2;

    ASSERT_EQ(m3.xx, 0.0);
    ASSERT_EQ(m3.xy, 0.0);
    ASSERT_EQ(m3.yx, 0.0);
    ASSERT_EQ(m3.yy, 0.0);
}

TEST(Linal, MatrixProduct)
{
    Matrix<2, 2> m1({
        1.0, 2.0,
        3.0, 4.0
    });

    Matrix<2, 1> m2({
        5.0, 6.0
    });

    auto m3 = m1 * m2;

    ASSERT_EQ(m3(0, 0), 17.0);
    ASSERT_EQ(m3(1, 0), 39.0);
}

TEST(Linal, MatrixProductSquare)
{
    Matrix<2, 2, Matrix22Container> m1({
        1.0, 2.0,
        3.0, 4.0
    });

    auto m2 = m1;

    auto m3 = m1 * m2;

    ASSERT_EQ(m3.xx, 7.0);
    ASSERT_EQ(m3.xy, 10.0);
    ASSERT_EQ(m3.yx, 15.0);
    ASSERT_EQ(m3.yy, 22.0);
}

TEST(Linal, MatrixEqual)
{
    Matrix<2, 3> m1({
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0
    });

    auto m2 = m1;

    auto m3 = m1*2;

    ASSERT_EQ(m1, m2);
    ASSERT_NE(m1, m3);
}

TEST(Linal, MatrixScalarMultiplication)
{
    Matrix<2, 2, Matrix22Container> m({
        1.0, 2.0,
        3.0, 4.0
    });

    m = 1*m*2;

    ASSERT_EQ(m.xx, 2.0);
    ASSERT_EQ(m.xy, 4.0);
    ASSERT_EQ(m.yx, 6.0);
    ASSERT_EQ(m.yy, 8.0);
}

TEST(Linal, MatrixScalarDivision)
{
    Matrix<2, 2, Matrix22Container> m({
        2.0, 4.0,
        6.0, 8.0
    });

    m = m/2;

    ASSERT_EQ(m.xx, 1.0);
    ASSERT_EQ(m.xy, 2.0);
    ASSERT_EQ(m.yx, 3.0);
    ASSERT_EQ(m.yy, 4.0);
}

TEST(Linal, MatrixTranspose)
{
    Matrix<2, 3> m1({
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0
    });

    auto m2 = m1.transpose();

    ASSERT_EQ(m2(0, 0), 1.0);
    ASSERT_EQ(m2(1, 0), 2.0);
    ASSERT_EQ(m2(2, 0), 3.0);
    ASSERT_EQ(m2(0, 1), 4.0);
    ASSERT_EQ(m2(1, 1), 5.0);
    ASSERT_EQ(m2(2, 1), 6.0);

    auto m3 = m2.transpose();

    ASSERT_EQ(m1, m3);
}

TEST(Linal, MatrixTransposeInplace)
{
    Matrix<2, 2> m1({
        1.0, 2.0,
        3.0, 4.0
    });

    auto m2 = m1;

    m1.transposeInplace();

    ASSERT_EQ(m1(0, 0), 1.0);
    ASSERT_EQ(m1(1, 0), 2.0);
    ASSERT_EQ(m1(0, 1), 3.0);
    ASSERT_EQ(m1(1, 1), 4.0);

    m1.transposeInplace();

    ASSERT_EQ(m1, m2);
}

TEST(Linal, MatrixInvert)
{
    Matrix<2, 2, Matrix22Container> m1({
        1.0, 2.0,
        1.0, 4.0
    });
    
    Matrix<2, 2, Matrix22Container> r({
        2.0, -1.0,
        -0.5, 0.5
    });
    
    Matrix<2, 2> i({
        1.0, 0.0,
        0.0, 1.0
    });


    auto m2 = m1.invert();
    
    ASSERT_EQ(m2 ,r);

    auto m3 = m2.invert();

    ASSERT_EQ(m1, m3);

    ASSERT_EQ(m1*m2, i);
}

TEST(Linal, MatrixInvertInplace)
{
    Matrix<2, 2, Matrix22Container> m1({
        1.0, 2.0,
        1.0, 4.0
    });

    auto m2 = m1;
    
    Matrix<2, 2, Matrix22Container> r({
        2.0, -1.0,
        -0.5, 0.5
    });
    
    Matrix<2, 2> i({
        1.0, 0.0,
        0.0, 1.0
    });


    m1.invertInplace();
    
    ASSERT_EQ(m1 ,r);
    
    ASSERT_EQ(m1*m2, i);

    m1.invertInplace();

    ASSERT_EQ(m1, m2);
}
