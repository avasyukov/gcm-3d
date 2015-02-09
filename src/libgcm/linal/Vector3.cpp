#include "libgcm/linal/Vector3.hpp"

using namespace gcm;
using namespace gcm::linal;

real gcm::linal::vectorLength(const Vector3& v)
{
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

Vector3 gcm::linal::vectorNormalize(const Vector3& v)
{
    auto l = vectorLength(v);

    assert_gt(l, 0.0);

    return v/l;
};

real gcm::linal::vectorNormalizeInplace(Vector3& v)
{
    auto l = vectorLength(v);

    assert_gt(l, 0.0);

    v.x /= l;
    v.y /= l;
    v.z /= l;

    return l;
};

real gcm::linal::dotProduct(const Vector3& v1, const Vector3& v2)
{
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

Vector3 gcm::linal::crossProduct(const Vector3& v1, const Vector3& v2)
{
    return Vector3({v1.y*v2.z-v1.z*v2.y, v1.z*v2.x-v1.x*v2.z, v1.x*v2.y-v1.y*v2.x});
}
        
Vector3 gcm::linal::operator*(const Matrix33& m, const Vector3& v)
{
    return operator*<3, 3, 1, Matrix33Container, Vector3Container, Vector3Container>(m, v);
}
