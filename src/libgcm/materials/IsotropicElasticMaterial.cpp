#include "libgcm/materials/IsotropicElasticMaterial.h"
#include "libgcm/util/RheologyMatrix3D.h"

gcm::IsotropicElasticMaterial::IsotropicElasticMaterial(string name, gcm_real rho, gcm_real crackThreshold, gcm_real la, gcm_real mu) : Material(name, rho, crackThreshold), la(la), mu(mu)
{
}

gcm::IsotropicElasticMaterial::~IsotropicElasticMaterial()
{
}

gcm_real gcm::IsotropicElasticMaterial::getLambda() const
{
    return la;
}

gcm_real gcm::IsotropicElasticMaterial::getMu() const
{
    return mu;
}

//void gcm::IsotropicElasticMaterial::prepareRheologyMatrixX(const CalcNode& node)
//{
////    matrix.prepareMatrix({la, mu, rho}, stage);
//}
//
//void gcm::IsotropicElasticMaterial::prepareRheologyMatrixY(const CalcNode& node)
//{
////    matrix.prepareMatrix({la, mu, rho}, stage);
//}
//
//void gcm::IsotropicElasticMaterial::prepareRheologyMatrixZ(const CalcNode& node)
//{
////    matrix.prepareMatrix({la, mu, rho}, stage);
//}

ElasticMatrix3D& gcm::IsotropicElasticMaterial::getRheologyMatrix()
{
    return matrix;
}
