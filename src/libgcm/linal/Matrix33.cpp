#include "libgcm/linal/Matrix33.hpp"
#include "libgcm/linal/RotationMatrix.hpp"
#include "libgcm/util/Assertion.hpp"

using namespace gcm;
using namespace gcm::linal;

real gcm::linal::determinant(const Matrix33& m)
{
    return m.a11*(m.a22*m.a33-m.a23*m.a32) - m.a12*(m.a21*m.a33-m.a23*m.a31) + m.a13*(m.a21*m.a32-m.a22*m.a31);
};

Matrix33 gcm::linal::rotate(const Matrix33& m, real a1, real a2, real a3)
{
	return getZRotationMatrix(-a3) * getYRotationMatrix(-a2) * getXRotationMatrix(-a1) * m * getXRotationMatrix(a1) * getYRotationMatrix(a2) * getZRotationMatrix(a3);
};
