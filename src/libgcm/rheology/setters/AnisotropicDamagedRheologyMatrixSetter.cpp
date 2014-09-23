#include "libgcm/rheology/setters/AnisotropicDamagedRheologyMatrixSetter.hpp"

using namespace gcm;

unsigned int AnisotropicDamagedRheologyMatrixSetter::getNumberOfStates() const
{
    return 0;
}

unsigned int AnisotropicDamagedRheologyMatrixSetter::getStateForNode(const ICalcNode& node) const
{
    return 0;
}

void AnisotropicDamagedRheologyMatrixSetter::setX(gcm_matrix& a, const MaterialPtr& material, const ICalcNode& node)
{
    a.clear();

    auto damage = node.getDamageMeasure();
    auto p = material->getRheologyProperties();
    auto rho = material->getRho();

    a(0,3) = a(1,4) = a(2,5) = -1.0/rho;

	a(3,0) = -p.c11*exp(-damage);	a(3,1) = -p.c16*exp(-damage);	a(3,2) = -p.c15*exp(-damage);
	a(4,0) = -p.c16*exp(-damage);	a(4,1) = -p.c66*exp(-damage);	a(4,2) = -p.c56*exp(-damage);
	a(5,0) = -p.c15*exp(-damage);	a(5,1) = -p.c56*exp(-damage);	a(5,2) = -p.c55*exp(-damage);
	a(6,0) = -p.c12*exp(-damage);	a(6,1) = -p.c26*exp(-damage);	a(6,2) = -p.c25*exp(-damage);
	a(7,0) = -p.c14*exp(-damage);	a(7,1) = -p.c46*exp(-damage);	a(7,2) = -p.c45*exp(-damage);
	a(8,0) = -p.c13*exp(-damage);	a(8,1) = -p.c36*exp(-damage);	a(8,2) = -p.c35*exp(-damage);
}

void AnisotropicDamagedRheologyMatrixSetter::setY(gcm_matrix& a, const MaterialPtr& material, const ICalcNode& node)
{
    a.clear();

    auto damage = node.getDamageMeasure();
    auto p = material->getRheologyProperties();
    auto rho = material->getRho();

    a(0,4) = a(1,6) = a(2,7) = -1.0/rho;

	a(3,0) = -p.c16*exp(-damage);	a(3,1) = -p.c12*exp(-damage);	a(3,2) = -p.c14*exp(-damage);
	a(4,0) = -p.c66*exp(-damage);	a(4,1) = -p.c26*exp(-damage);	a(4,2) = -p.c46*exp(-damage);
	a(5,0) = -p.c56*exp(-damage);	a(5,1) = -p.c25*exp(-damage);	a(5,2) = -p.c45*exp(-damage);
	a(6,0) = -p.c26*exp(-damage);	a(6,1) = -p.c22*exp(-damage);	a(6,2) = -p.c24*exp(-damage);
	a(7,0) = -p.c46*exp(-damage);	a(7,1) = -p.c24*exp(-damage);	a(7,2) = -p.c44*exp(-damage);
	a(8,0) = -p.c36*exp(-damage);	a(8,1) = -p.c23*exp(-damage);	a(8,2) = -p.c34*exp(-damage);
}

void AnisotropicDamagedRheologyMatrixSetter::setZ(gcm_matrix& a, const MaterialPtr& material, const ICalcNode& node)
{
    a.clear();

    auto damage = node.getDamageMeasure();
    auto p = material->getRheologyProperties();
    auto rho = material->getRho();

    a(0,5) = a(1,7) = a(2,8) = -1.0/rho;

	a(3,0) = -p.c15*exp(-damage);	a(3,1) = -p.c14*exp(-damage);	a(3,2) = -p.c13*exp(-damage);
	a(4,0) = -p.c56*exp(-damage);	a(4,1) = -p.c46*exp(-damage);	a(4,2) = -p.c36*exp(-damage);
	a(5,0) = -p.c55*exp(-damage);	a(5,1) = -p.c45*exp(-damage);	a(5,2) = -p.c35*exp(-damage);
	a(6,0) = -p.c25*exp(-damage);	a(6,1) = -p.c24*exp(-damage);	a(6,2) = -p.c23*exp(-damage);
	a(7,0) = -p.c45*exp(-damage);	a(7,1) = -p.c44*exp(-damage);	a(7,2) = -p.c34*exp(-damage);
	a(8,0) = -p.c35*exp(-damage);	a(8,1) = -p.c34*exp(-damage);	a(8,2) = -p.c33*exp(-damage);
}
