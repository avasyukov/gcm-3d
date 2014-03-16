#include "AnisotropicElasticMaterialLoader.h"
#include "materials/IAnisotropicElasticMaterial.h"

#include <cstdlib>
#include <cerrno>
#include <utility>
#include <sstream>

#include "materials/IAnisotropicElasticMaterial.h"
#include "util/helpers.h"

const string launcher::AnisotropicElasticMaterialLoader::TYPE = "AEM";

gcm::AnisotropicElasticMaterial* launcher::AnisotropicElasticMaterialLoader::load(xml::Node desc)
{
    AttrList attrs = desc.getAttributes();

    string name = getAttributeByName(attrs, "name");
    string type = getAttributeByName(attrs, "type");
    assert(type == TYPE);

    int _errno = errno;
    errno = 0;

    IAnisotropicElasticMaterial::RheologyParameters params;

    gcm_real rho = s2r(desc.getChildByName("rho").getTextContent());
    gcm_real crackThreshold = s2r(desc.getChildByName("crackThreshold").getTextContent());

    int k = 0;
    for (int i = 1; i <= 6; i++)
        for (int j = i; j <= 6; j++) {
            stringstream cxx;
            cxx << "c" << i << j;
            params.values[k++] = s2r(desc.getChildByName(cxx.str()).getTextContent());
        }

    swap(errno, _errno);

    if (rho <= 0.0 || _errno)
        THROW_INVALID_INPUT("Seems xml snippet does not contain valid rheology parameters.");

    for (k = 0; k < ANISOTROPIC_ELASTIC_MATERIALS_PARAMETERS_NUM; k++)
        if (params.values[k] < 0.0)
            THROW_INVALID_INPUT("Seems xml snippet does not contain valid rheology parameters.");

    return new AnisotropicElasticMaterial(name, rho, crackThreshold, params);
}