#include "libgcm/linal/RotationMatrix.hpp"

#include <cmath>

using namespace gcm;
using namespace gcm::linal;

// Explicitly cast double to real to avoid compiler warning about narrowing conversion inside {}
#define rcos(a) static_cast<real>(cos(a))
#define rsin(a) static_cast<real>(sin(a))

Matrix33 gcm::linal::getXRotationMatrix(real angle)
{
    return Matrix33({
        1.0,  0.0        , 0.0        ,
        0.0,  rcos(angle), rsin(angle),
        0.0, -rsin(angle), rcos(angle)
    });
}

Matrix33 gcm::linal::getYRotationMatrix(real angle)
{
    return Matrix33({
        rcos(angle), 0.0, -rsin(angle),
        0.0        , 1.0,  0.0,
        rsin(angle), 0.0,  rcos(angle)
    });
}

Matrix33 gcm::linal::getZRotationMatrix(real angle)
{
    return Matrix33({
         rcos(angle), rsin(angle), 0.0,
        -rsin(angle), rcos(angle), 0.0,
         0.0        , 0.0        , 1.0
    });
}
