#include "libgcm/rheology/RheologyMatrix.hpp"

#include "libgcm/util/Assertion.hpp"

using namespace gcm;
#include <iostream>

RheologyMatrix::RheologyMatrix(const MaterialPtr& material, const SetterPtr& setter, const DecomposerPtr& decomposer): material(material), setter(setter), decomposer(decomposer)
{
    auto n = setter->getNumberOfStates(); 
    if (n == 0)
    {
        n = 1;
        immutable = false;
    }
    for (auto& m: matrices)
    {   
        m.a = new gcm_matrix[n];
        m.u = new gcm_matrix[n];
        m.l = new gcm_matrix[n];
        m.u1 = new gcm_matrix[n];
        m.cached = new bool[n];
        for (unsigned int j = 0; j < n; j++)
            m.cached[j] = false;
    }
}

RheologyMatrix::~RheologyMatrix()
{
    for (auto m: matrices)
    {
        delete[] m.a;
        delete[] m.u;
        delete[] m.l;
        delete[] m.u1;
        delete[] m.cached;
    }
}

gcm_real RheologyMatrix::getMaxEigenvalue() const
{
    float res = 0;
    for (unsigned i = 0; i < GCM_MATRIX_SIZE; ++i)
    {
        gcm_real la = getL(i, i);
        if(la > res)
            res = la;
    }
    return res;
}

gcm_real RheologyMatrix::getMinEigenvalue() const
{
    float res = numeric_limits<gcm_real>::infinity();
    for (unsigned i = 0; i < GCM_MATRIX_SIZE; ++i)
    {
        gcm_real la = getL(i, i);
        if(la > 0 && la < res)
            res = la;
    }
    return res;
}

const gcm_matrix& RheologyMatrix::getA() const
{
    assert_true(matrices[direction].cached[index]);
    return matrices[direction].a[index];
}

gcm_real RheologyMatrix::getA(unsigned int i, unsigned int j) const
{
    return getA().get(i, j);
}

const gcm_matrix& RheologyMatrix::getL() const
{
    assert_true(matrices[direction].cached[index]);
    return matrices[direction].l[index];
}

gcm_real RheologyMatrix::getL(unsigned int i, unsigned int j) const
{
    return getL().get(i, j);
}

const gcm_matrix& RheologyMatrix::getU() const
{
    assert_true(matrices[direction].cached[index]);
    return matrices[direction].u[index];
}

gcm_real RheologyMatrix::getU(unsigned int i, unsigned int j) const
{
    return getU().get(i, j);
}

const gcm_matrix& RheologyMatrix::getU1() const
{
    assert_true(matrices[direction].cached[index]);
    return matrices[direction].u1[index];
}

gcm_real RheologyMatrix::getU1(unsigned int i, unsigned int j) const
{
    return getU1().get(i, j);
}

const MaterialPtr& RheologyMatrix::getMaterial() const
{
    return material;
}

void RheologyMatrix::decomposeX(const ICalcNode& node)
{
    decompose(node, 0);
}

void RheologyMatrix::decomposeY(const ICalcNode& node)
{
    decompose(node, 1);
}

void RheologyMatrix::decomposeZ(const ICalcNode& node)
{
    decompose(node, 2);
}

void RheologyMatrix::decompose(const ICalcNode& node, unsigned int direction)
{
    assert_le(direction, 2);

    auto s = setter->getStateForNode(node);
    if (!matrices[direction].cached[s] || !immutable)
    {
        switch(direction)
        {
            case 0: setter->setX(matrices[0].a[s], material, node); break;
            case 1: setter->setY(matrices[1].a[s], material, node); break;
            case 2: setter->setZ(matrices[2].a[s], material, node); break;

        }
        switch (direction)
        {
            case 0: decomposer->decomposeX(matrices[0].a[s], matrices[0].u[s], matrices[0].l[s], matrices[0].u1[s]); break;
            case 1: decomposer->decomposeY(matrices[1].a[s], matrices[1].u[s], matrices[1].l[s], matrices[1].u1[s]); break;
            case 2: decomposer->decomposeZ(matrices[2].a[s], matrices[2].u[s], matrices[2].l[s], matrices[2].u1[s]); break;
        }
        matrices[direction].cached[s] = true;
    }

    this->direction = direction;
    this->index = s;
}


// ====================================================================================================================

/*
void RheologyMatrix::CreateGeneralizedMatrix(float la, float mu, float ro,
                                                    float qjx, float qjy, float qjz)
{

    zero_all();

    A(0,0) = 0;                    A(0,1) = 0;                    A(0,2) = 0;
    A(0,3) = -qjx/ro;            A(0,4) = -qjy/ro;            A(0,5) = -qjz/ro;
    A(0,6) = 0;                    A(0,7) = 0;                    A(0,8) = 0;

    A(1,0) = 0;                    A(1,1) = 0;                    A(1,2) = 0;
    A(1,3) = 0;                    A(1,4) = -qjx/ro;            A(1,5) = 0;
    A(1,6) = -qjy/ro;            A(1,7) = -qjz/ro;            A(1,8) = 0;

    A(2,0) = 0;                    A(2,1) = 0;                    A(2,2) = 0;
    A(2,3) = 0;                    A(2,4) = 0;                    A(2,5) = -qjx/ro;
    A(2,6) = 0;                    A(2,7) = -qjy/ro;            A(2,8) = -qjz/ro;

    A(3,0) = -(la+2*mu)*qjx;    A(3,1) = -la*qjy;            A(3,2) = -la*qjz;
    A(3,3) = 0;                    A(3,4) = 0;                    A(3,5) = 0;
    A(3,6) = 0;                    A(3,7) = 0;                    A(3,8) = 0;

    A(4,0) = -mu*qjy;            A(4,1) = -mu*qjx;            A(4,2) = 0;
    A(4,3) = 0;                    A(4,4) = 0;                    A(4,5) = 0;
    A(4,6) = 0;                    A(4,7) = 0;                    A(4,8) = 0;

    A(5,0) = -mu*qjz;            A(5,1) = 0;                    A(5,2) = -mu*qjx;
    A(5,3) = 0;                    A(5,4) = 0;                    A(5,5) = 0;
    A(5,6) = 0;                    A(5,7) = 0;                    A(5,8) = 0;

    A(6,0) = -la*qjx;            A(6,1) = -(la+2*mu)*qjy;    A(6,2) = -la*qjz;
    A(6,3) = 0;                    A(6,4) = 0;                    A(6,5) = 0;
    A(6,6) = 0;                    A(6,7) = 0;                    A(6,8) = 0;

    A(7,0) = 0;                    A(7,1) = -mu*qjz;            A(7,2) = -mu*qjy;
    A(7,3) = 0;                    A(7,4) = 0;                    A(7,5) = 0;
    A(7,6) = 0;                    A(7,7) = 0;                    A(7,8) = 0;

    A(8,0) = -la*qjx;            A(8,1) = -la*qjy;            A(8,2) = -(la+2*mu)*qjz;
    A(8,3) = 0;                    A(8,4) = 0;                    A(8,5) = 0;
    A(8,6) = 0;                    A(8,7) = 0;                    A(8,8) = 0;

    float l = vectorNorm(qjx, qjy, qjz);
    float scale = 1 / l;

    n[0][0] = qjx * scale;
    n[0][1] = qjy * scale;
    n[0][2] = qjz * scale;

    createLocalBasis( n[0], n[1], n[2] );

    float c1 = sqrt((la+2*mu)/ro);
    float c2 = sqrt(mu/ro);
    float c3 = sqrt(la*la/(ro*(la+2*mu)));

    L(0,0) = c1 * l;
    L(1,1) = -c1 * l;
    L(2,2) = c2 * l;
    L(3,3) = -c2 * l;
    L(4,4) = c2 * l;
    L(5,5) = -c2 * l;
    L(6,6) = 0;
    L(7,7) = 0;
    L(8,8) = 0;

    float I[6];
    float N00[6];
    float N01[6];
    float N02[6];
    float N11[6];
    float N12[6];
    float N22[6];

    I[0] = 1; I[1] = 0; I[2] = 0; I[3] = 1; I[4] = 0; I[5] = 1;
    createMatrixN(0,0,N00);
    createMatrixN(0,1,N01);
    createMatrixN(0,2,N02);
    createMatrixN(1,1,N11);
    createMatrixN(1,2,N12);
    createMatrixN(2,2,N22);

    U1(0,0) = n[0][0];
    U1(1,0) = n[0][1];
    U1(2,0) = n[0][2];
    for(int i = 0; i < 6; i++)
        U1(3+i,0) = -ro*((c1-c3)*N00[i] + c3*I[i]);

    U1(0,1) = n[0][0];
    U1(1,1) = n[0][1];
    U1(2,1) = n[0][2];
    for(int i = 0; i < 6; i++)
        U1(3+i,1) = ro*((c1-c3)*N00[i] + c3*I[i]);

    U1(0,2) = n[1][0];
    U1(1,2) = n[1][1];
    U1(2,2) = n[1][2];
    for(int i = 0; i < 6; i++)
        U1(3+i,2) = -2*ro*c2*N01[i];

    U1(0,3) = n[1][0];
    U1(1,3) = n[1][1];
    U1(2,3) = n[1][2];
    for(int i = 0; i < 6; i++)
        U1(3+i,3) = 2*ro*c2*N01[i];

    U1(0,4) = n[2][0];
    U1(1,4) = n[2][1];
    U1(2,4) = n[2][2];
    for(int i = 0; i < 6; i++)
        U1(3+i,4) = -2*ro*c2*N02[i];

    U1(0,5) = n[2][0];
    U1(1,5) = n[2][1];
    U1(2,5) = n[2][2];
    for(int i = 0; i < 6; i++)
        U1(3+i,5) = 2*ro*c2*N02[i];

    U1(0,6) = 0;
    U1(1,6) = 0;
    U1(2,6) = 0;
    for(int i = 0; i < 6; i++)
        U1(3+i,6) = 4*N12[i];

    U1(0,7) = 0;
    U1(1,7) = 0;
    U1(2,7) = 0;
    for(int i = 0; i < 6; i++)
        U1(3+i,7) = N11[i] - N22[i];

    U1(0,8) = 0;
    U1(1,8) = 0;
    U1(2,8) = 0;
    for(int i = 0; i < 6; i++)
        U1(3+i,8) = I[i] - N00[i];

    for(int i = 0; i < GCM_MATRIX_SIZE; i++)
        for(int j = 0; j < GCM_MATRIX_SIZE; j++)
            U1(i,j) /= 2;

    U(0, 0) = n[0][0];
    U(0, 1) = n[0][1];
    U(0, 2) = n[0][2];
    U(0, 3) = -N00[0]/(c1*ro);
    U(0, 4) = -2*N00[1]/(c1*ro);
    U(0, 5) = -2*N00[2]/(c1*ro);
    U(0, 6) = -N00[3]/(c1*ro);
    U(0, 7) = -2*N00[4]/(c1*ro);
    U(0, 8) = -N00[5]/(c1*ro);

    U(1, 0) = n[0][0];
    U(1, 1) = n[0][1];
    U(1, 2) = n[0][2];
    U(1, 3) = N00[0]/(c1*ro);
    U(1, 4) = 2*N00[1]/(c1*ro);
    U(1, 5) = 2*N00[2]/(c1*ro);
    U(1, 6) = N00[3]/(c1*ro);
    U(1, 7) = 2*N00[4]/(c1*ro);
    U(1, 8) = N00[5]/(c1*ro);

    U(2, 0) = n[1][0];
    U(2, 1) = n[1][1];
    U(2, 2) = n[1][2];
    U(2, 3) = -N01[0]/(c2*ro);
    U(2, 4) = -2*N01[1]/(c2*ro);
    U(2, 5) = -2*N01[2]/(c2*ro);
    U(2, 6) = -N01[3]/(c2*ro);
    U(2, 7) = -2*N01[4]/(c2*ro);
    U(2, 8) = -N01[5]/(c2*ro);

    U(3, 0) = n[1][0];
    U(3, 1) = n[1][1];
    U(3, 2) = n[1][2];
    U(3, 3) = N01[0]/(c2*ro);
    U(3, 4) = 2*N01[1]/(c2*ro);
    U(3, 5) = 2*N01[2]/(c2*ro);
    U(3, 6) = N01[3]/(c2*ro);
    U(3, 7) = 2*N01[4]/(c2*ro);
    U(3, 8) = N01[5]/(c2*ro);

    U(4, 0) = n[2][0];
    U(4, 1) = n[2][1];
    U(4, 2) = n[2][2];
    U(4, 3) = -N02[0]/(c2*ro);
    U(4, 4) = -2*N02[1]/(c2*ro);
    U(4, 5) = -2*N02[2]/(c2*ro);
    U(4, 6) = -N02[3]/(c2*ro);
    U(4, 7) = -2*N02[4]/(c2*ro);
    U(4, 8) = -N02[5]/(c2*ro);

    U(5, 0) = n[2][0];
    U(5, 1) = n[2][1];
    U(5, 2) = n[2][2];
    U(5, 3) = N02[0]/(c2*ro);
    U(5, 4) = 2*N02[1]/(c2*ro);
    U(5, 5) = 2*N02[2]/(c2*ro);
    U(5, 6) = N02[3]/(c2*ro);
    U(5, 7) = 2*N02[4]/(c2*ro);
    U(5, 8) = N02[5]/(c2*ro);

    U(6, 0) = 0;
    U(6, 1) = 0;
    U(6, 2) = 0;
    U(6, 3) = N12[0];
    U(6, 4) = 2*N12[1];
    U(6, 5) = 2*N12[2];
    U(6, 6) = N12[3];
    U(6, 7) = 2*N12[4];
    U(6, 8) = N12[5];

    U(7, 0) = 0;
    U(7, 1) = 0;
    U(7, 2) = 0;
    U(7, 3) = (N11[0]-N22[0]);
    U(7, 4) = 2*(N11[1]-N22[1]);
    U(7, 5) = 2*(N11[2]-N22[2]);
    U(7, 6) = (N11[3]-N22[3]);
    U(7, 7) = 2*(N11[4]-N22[4]);
    U(7, 8) = (N11[5]-N22[5]);

    U(8, 0) = 0;
    U(8, 1) = 0;
    U(8, 2) = 0;
    U(8, 3) = (N11[0]+N22[0]-2*la*N00[0]/(la+2*mu));
    U(8, 4) = 2*(N11[1]+N22[1]-2*la*N00[1]/(la+2*mu));
    U(8, 5) = 2*(N11[2]+N22[2]-2*la*N00[2]/(la+2*mu));
    U(8, 6) = (N11[3]+N22[3]-2*la*N00[3]/(la+2*mu));
    U(8, 7) = 2*(N11[4]+N22[4]-2*la*N00[4]/(la+2*mu));
    U(8, 8) = (N11[5]+N22[5]-2*la*N00[5]/(la+2*mu));

};


void RheologyMatrix::createMatrixN(int i, int j, float *res)
{
    *res = (n[i][0]*n[j][0] + n[j][0]*n[i][0])/2;
    *(res+1) = (n[i][0]*n[j][1] + n[j][0]*n[i][1])/2;
    *(res+2) = (n[i][0]*n[j][2] + n[j][0]*n[i][2])/2;
    *(res+3) = (n[i][1]*n[j][1] + n[j][1]*n[i][1])/2;
    *(res+4) = (n[i][1]*n[j][2] + n[j][1]*n[i][2])/2;
    *(res+5) = (n[i][2]*n[j][2] + n[j][2]*n[i][2])/2;
};
*/
