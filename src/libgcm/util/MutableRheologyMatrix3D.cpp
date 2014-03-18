#include "MutableRheologyMatrix3D.h"

gcm::MutableRheologyMatrix3D::MutableRheologyMatrix3D()
{
}

gcm_real gcm::MutableRheologyMatrix3D::getMaxEigenvalue() const
{
    float res = 0;
    for (unsigned i = 0; i < GCM_MATRIX_SIZE; ++i)
    {
        gcm_real la = RheologyMatrix3D::getL(i, i);
        if(la > res)
            res = la;
    }
    return res;
}

gcm_real gcm::MutableRheologyMatrix3D::getMinEigenvalue() const
{
    float res = numeric_limits<gcm_real>::infinity();
    for (unsigned i = 0; i < GCM_MATRIX_SIZE; ++i)
    {
        gcm_real la = RheologyMatrix3D::getL(i, i);
        if(la > 0 && la < res)
            res = la;
    }
    return res;
}

const gcm_matrix& gcm::MutableRheologyMatrix3D::getA() const
{
    return A;
}

const gcm_matrix& gcm::MutableRheologyMatrix3D::getL() const
{
    return L;
}

const gcm_matrix& gcm::MutableRheologyMatrix3D::getU() const
{
    return U;
}

const gcm_matrix& gcm::MutableRheologyMatrix3D::getU1() const
{
    return U1;
}