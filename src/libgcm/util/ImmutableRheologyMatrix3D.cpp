#include "libgcm/util/ImmutableRheologyMatrix3D.h"

gcm::ImmutableRheologyMatrix3D::ImmutableRheologyMatrix3D(): matrixCreated {false, false, false}, currentMatrix(0)
{
}

gcm_real gcm::ImmutableRheologyMatrix3D::getMaxEigenvalue() const
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

gcm_real gcm::ImmutableRheologyMatrix3D::getMinEigenvalue() const
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

const gcm_matrix& gcm::ImmutableRheologyMatrix3D::getA() const
{
    return A[currentMatrix];
}

const gcm_matrix& gcm::ImmutableRheologyMatrix3D::getL() const
{
    return L[currentMatrix];
}

const gcm_matrix& gcm::ImmutableRheologyMatrix3D::getU() const
{
    return U[currentMatrix];
}

const gcm_matrix& gcm::ImmutableRheologyMatrix3D::getU1() const
{
    return U1[currentMatrix];
}

void gcm::ImmutableRheologyMatrix3D::createAx(const ICalcNode& node)
{
    currentMatrix = 0;
    if (!matrixCreated[currentMatrix])
    {
        initializeAx(node.getMaterial(), A[currentMatrix], L[currentMatrix], U[currentMatrix], U1[currentMatrix]);
        matrixCreated[currentMatrix] = true;
    }
}

void gcm::ImmutableRheologyMatrix3D::createAy(const ICalcNode& node)
{
    currentMatrix = 1;
    if (!matrixCreated[currentMatrix])
    {
        initializeAy(node.getMaterial(), A[currentMatrix], L[currentMatrix], U[currentMatrix], U1[currentMatrix]);
        matrixCreated[currentMatrix] = true;
    }
}

void gcm::ImmutableRheologyMatrix3D::createAz(const ICalcNode& node)
{
    currentMatrix = 2;
    if (!matrixCreated[currentMatrix])
    {
        initializeAz(node.getMaterial(), A[currentMatrix], L[currentMatrix], U[currentMatrix], U1[currentMatrix]);
        matrixCreated[currentMatrix] = true;
    }
}
