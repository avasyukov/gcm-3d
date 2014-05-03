#include "libgcm/rheology/setters/AnisotropicRheologyMatrixSetter.hpp"

using namespace gcm;

unsigned int AnisotropicRheologyMatrixSetter::getNumberOfStates() const
{
    return 1;
}

unsigned int AnisotropicRheologyMatrixSetter::getStateForNode(const ICalcNode& node) const
{
    return 0;
}

void AnisotropicRheologyMatrixSetter::setX(gcm_matrix& a, const MaterialPtr& material)
{
    a.clear();

    auto p = material->getRheologyProperties();
    auto rho = material->getRho();

    a(0,3) = a(1,4) = a(2,5) = -1.0/rho;

	a(3,0) = -p.c11;	a(3,1) = -p.c16;	a(3,2) = -p.c15;
	a(4,0) = -p.c16;	a(4,1) = -p.c66;	a(4,2) = -p.c56;
	a(5,0) = -p.c15;	a(5,1) = -p.c56;	a(5,2) = -p.c55;
	a(6,0) = -p.c12;	a(6,1) = -p.c26;	a(6,2) = -p.c25;
	a(7,0) = -p.c14;	a(7,1) = -p.c46;	a(7,2) = -p.c45;
	a(8,0) = -p.c13;	a(8,1) = -p.c36;	a(8,2) = -p.c35;
}

void AnisotropicRheologyMatrixSetter::setY(gcm_matrix& a, const MaterialPtr& material)
{
    a.clear();

    auto p = material->getRheologyProperties();
    auto rho = material->getRho();

    a(0,4) = a(1,6) = a(2,7) = -1.0/rho;

	a(3,0) = -p.c16;	a(3,1) = -p.c12;	a(3,2) = -p.c14;
	a(4,0) = -p.c66;	a(4,1) = -p.c26;	a(4,2) = -p.c46;
	a(5,0) = -p.c56;	a(5,1) = -p.c25;	a(5,2) = -p.c45;
	a(6,0) = -p.c26;	a(6,1) = -p.c22;	a(6,2) = -p.c24;
	a(7,0) = -p.c46;	a(7,1) = -p.c24;	a(7,2) = -p.c44;
	a(8,0) = -p.c36;	a(8,1) = -p.c23;	a(8,2) = -p.c34;
}

void AnisotropicRheologyMatrixSetter::setZ(gcm_matrix& a, const MaterialPtr& material)
{
    a.clear();

    auto p = material->getRheologyProperties();
    auto rho = material->getRho();

    a(0,5) = a(1,7) = a(2,8) = -1.0/rho;

	a(3,0) = -p.c15;	a(3,1) = -p.c14;	a(3,2) = -p.c13;
	a(4,0) = -p.c56;	a(4,1) = -p.c46;	a(4,2) = -p.c36;
	a(5,0) = -p.c55;	a(5,1) = -p.c45;	a(5,2) = -p.c35;
	a(6,0) = -p.c25;	a(6,1) = -p.c24;	a(6,2) = -p.c23;
	a(7,0) = -p.c45;	a(7,1) = -p.c44;	a(7,2) = -p.c34;
	a(8,0) = -p.c35;	a(8,1) = -p.c34;	a(8,2) = -p.c33;
}
