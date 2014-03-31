#include "libgcm/materials/Material.h"

gcm::Material::Material(string name, gcm_real rho, gcm_real crackThreshold) : name(name), rho(rho), crackThreshold(crackThreshold)
{
}

gcm::Material::~Material()
{
}

const string& gcm::Material::getName() const
{
    return name;
}

gcm_real gcm::Material::getRho() const
{
    return rho;
}

gcm_real gcm::Material::getCrackThreshold() const
{
    return crackThreshold;
}

