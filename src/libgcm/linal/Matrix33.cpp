#include "libgcm/linal/Matrix33.hpp"

using namespace gcm;
using namespace gcm::linal;

Matrix33::Matrix33(): Matrix()
{
}

Matrix33::Matrix33(const Matrix33& m): Matrix(m)
{
}

Matrix33::Matrix33(std::initializer_list<real> values): Matrix(values)
{
}

Matrix33& Matrix33::operator=(const Matrix33& m)
{
    static_cast<Matrix>(*this) = m;
}

real Matrix33::determinant() const
{
    return a11*(a22*a33-a23*a32) - a12*(a21*a33-a23*a31) + a13*(a21*a32-a22*a31);
};
