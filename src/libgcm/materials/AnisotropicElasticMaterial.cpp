#include "libgcm/materials/AnisotropicElasticMaterial.h"

#include <cassert>

gcm::AnisotropicElasticMaterial::AnisotropicElasticMaterial(string name, gcm_real rho, gcm_real crackThreshold, RheologyParameters params) : Material(name, rho, crackThreshold), rheologyParameters(params)
{
}

gcm::AnisotropicElasticMaterial::~AnisotropicElasticMaterial()
{
}

const gcm::IAnisotropicElasticMaterial::RheologyParameters& gcm::AnisotropicElasticMaterial::getParameters() const
{
    return rheologyParameters;
}

//void gcm::AnisotropicElasticMaterial::prepareRheologyMatrixX(const CalcNode& node)
//{
////    matrix.prepareMatrix({}, stage);
//}
//
//void gcm::AnisotropicElasticMaterial::prepareRheologyMatrixY(const CalcNode& node)
//{
////    matrix.prepareMatrix({}, stage);
//}
//
//void gcm::AnisotropicElasticMaterial::prepareRheologyMatrixZ(const CalcNode& node)
//{
////    matrix.prepareMatrix({}, stage);
//}

AnisotropicMatrix3D& gcm::AnisotropicElasticMaterial::getRheologyMatrix()
{
    return matrix;
}
