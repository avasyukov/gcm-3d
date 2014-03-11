#include "IsotropicElasticMaterialLoader.h"

#include <cstdlib>
#include <cerrno>
#include <utility>

#include "util/helpers.h"

const string launcher::IsotropicElasticMaterialLoader::TYPE = "IEM";

gcm::IsotropicElasticMaterial* launcher::IsotropicElasticMaterialLoader::load(xml::Node desc)
{
        AttrList attrs = desc.getAttributes();
        
        string name = getAttributeByName(attrs, "name");
        string type = getAttributeByName(attrs, "type");
        assert(type == TYPE);
        
        int _errno = errno;
        errno = 0;
        gcm_real la = s2r(desc.getChildByName("la").getTextContent());
        gcm_real mu = s2r(desc.getChildByName("mu").getTextContent());
        gcm_real rho = s2r(desc.getChildByName("rho").getTextContent());
        gcm_real crackThreshold = s2r(desc.getChildByName("crackThreshold").getTextContent());
        swap(errno, _errno);
        
        if (la <= 0.0 || mu <= 0.0 || rho <= 0.0 || crackThreshold <= 0.0 || _errno)
            THROW_INVALID_INPUT("Seems xml snippet does not contain valid rheology parameters.");
        
        return new IsotropicElasticMaterial(name, rho, crackThreshold, la, mu);
}