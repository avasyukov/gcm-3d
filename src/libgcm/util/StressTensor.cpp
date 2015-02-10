#include "libgcm/util/StressTensor.hpp"
#include "libgcm/util/Assertion.hpp"
#include "libgcm/linal/RotationMatrix.hpp"
#include <cmath>

using namespace gcm;
using namespace gcm::linal;

StressTensor::StressTensor()
{
}

StressTensor::StressTensor(real xx, real xy, real xz, real yy, real yz, real zz): xx(xx), xy(xy), xz(xz), yy(yy), yz(yz), zz(zz)
{
}

real StressTensor::trace() const
{
    return xx + yy + zz;
}

void StressTensor::transform(const Matrix33& s)
{
    assert_ne(determinant(s), 0.0);

    Matrix33 result({
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0
    });

    Matrix33 m({
        xx, xy, xz,
        yx, yy, yz,
        zx, yz, zz
    });

    for (uint i = 0; i < 3; i++)
        for (uint j = 0; j < 3; j++)
            for (uint h1 = 0; h1 < 3; h1++)
                for (uint h2 = 0; h2 < 3; h2++)
                    result(i, j) += s(i, h1)*s(j, h2)*m(h1, h2);

    xx = result.xx;
    xy = result.xy;
    xz = result.xz;
    yy = result.yy;
    yz = result.yz;
    zz = result.zz;
}

void StressTensor::setToIsotropicPWave(const MaterialPtr& material, const Vector3& direction)
{
    Matrix33 m({
        -700.0,  0.0  ,  0.0  ,
         0.0  , -900.0,  0.0  ,
         0.0  ,  0.0  , -700.0
    });

    auto alpha = atan2(direction.y, direction.x) - M_PI/2;
    
    auto _direction = getZRotationMatrix(alpha)*direction;


    auto beta = atan2(_direction.z, _direction.y);

    auto s = getZRotationMatrix(-alpha) *getXRotationMatrix(-beta);

    transform(s);
}
