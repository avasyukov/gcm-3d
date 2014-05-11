#include "libgcm/rheology/setters/IsotropicRheologyMatrixSetter.hpp"

using namespace gcm;
unsigned int IsotropicRheologyMatrixSetter::getNumberOfStates() const
{
    return 1;
}

unsigned int IsotropicRheologyMatrixSetter::getStateForNode(const ICalcNode& node) const
{
    return 0;
}

void IsotropicRheologyMatrixSetter::setX(gcm_matrix& a, const MaterialPtr& material, const ICalcNode& node)
{
    a.clear();
    
    auto rho = material->getRho();
    auto mu = material->getMu();
    auto la = material->getLa();

    a(0, 3) = -1 / rho;
    a(1, 4) = -1 / rho;
    a(2, 5) = -1 / rho;
    a(3, 0) = -la - 2 * mu;
    a(4, 1) = -mu;
    a(5, 2) = -mu;
    a(6, 0) = -la;
    a(8, 0) = -la;
}

void IsotropicRheologyMatrixSetter::setY(gcm_matrix& a, const MaterialPtr& material, const ICalcNode& node)
{
    a.clear();
    
    auto rho = material->getRho();
    auto mu = material->getMu();
    auto la = material->getLa();
    
    a(0, 4) = -1 / rho;
    a(1, 6) = -1 / rho;
    a(2, 7) = -1 / rho;
    a(3, 1) = -la;
    a(4, 0) = -mu;
    a(6, 1) = -la - 2 * mu;
    a(7, 2) = -mu;
    a(8, 1) = -la;
}

void IsotropicRheologyMatrixSetter::setZ(gcm_matrix& a, const MaterialPtr& material, const ICalcNode& node)
{
    a.clear();
    
    auto rho = material->getRho();
    auto mu = material->getMu();
    auto la = material->getLa();
    
    a(0, 5) = -1 / rho;
    a(1, 7) = -1 / rho;
    a(2, 8) = -1 / rho;
    a(3, 2) = -la;
    a(5, 0) = -mu;
    a(6, 2) = -la;
    a(7, 1) = -mu;
    a(8, 2) = -la - 2 * mu;
}
