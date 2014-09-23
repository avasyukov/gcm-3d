#include "libgcm/rheology/setters/IsotropicDamagedRheologyMatrixSetter.hpp"

using namespace gcm;
unsigned int IsotropicDamagedRheologyMatrixSetter::getNumberOfStates() const
{
    return 0;
}

unsigned int IsotropicDamagedRheologyMatrixSetter::getStateForNode(const ICalcNode& node) const
{
    return 0;
}

void IsotropicDamagedRheologyMatrixSetter::setX(gcm_matrix& a, const MaterialPtr& material, const ICalcNode& node)
{
    a.clear();
    auto damage = node.getDamageMeasure();
    auto rho = material->getRho();
    auto mu = material->getMu()*exp( - damage );
    auto la = material->getLa()*exp( - damage );

    a(0, 3) = -1 / rho;
    a(1, 4) = -1 / rho;
    a(2, 5) = -1 / rho;
    a(3, 0) = -la - 2 * mu;
    a(4, 1) = -mu;
    a(5, 2) = -mu;
    a(6, 0) = -la;
    a(8, 0) = -la;
}

void IsotropicDamagedRheologyMatrixSetter::setY(gcm_matrix& a, const MaterialPtr& material, const ICalcNode& node)
{
    a.clear();

    auto damage = node.getDamageMeasure();    
    auto rho = material->getRho();
    auto mu = material->getMu()*exp( - damage );
    auto la = material->getLa()*exp( - damage );
    
    a(0, 4) = -1 / rho;
    a(1, 6) = -1 / rho;
    a(2, 7) = -1 / rho;
    a(3, 1) = -la;
    a(4, 0) = -mu;
    a(6, 1) = -la - 2 * mu;
    a(7, 2) = -mu;
    a(8, 1) = -la;
}

void IsotropicDamagedRheologyMatrixSetter::setZ(gcm_matrix& a, const MaterialPtr& material, const ICalcNode& node)
{
    a.clear();
    
    auto damage = node.getDamageMeasure();
    auto rho = material->getRho();
    auto mu = material->getMu()*exp( - damage );
    auto la = material->getLa()*exp( - damage );
    
    a(0, 5) = -1 / rho;
    a(1, 7) = -1 / rho;
    a(2, 8) = -1 / rho;
    a(3, 2) = -la;
    a(5, 0) = -mu;
    a(6, 2) = -la;
    a(7, 1) = -mu;
    a(8, 2) = -la - 2 * mu;
}
