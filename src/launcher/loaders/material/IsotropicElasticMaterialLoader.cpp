#include "launcher/loaders/material/IsotropicElasticMaterialLoader.h"

#include <cstdlib>
#include <cerrno>
#include <utility>
#include <boost/lexical_cast.hpp>

using boost::lexical_cast;

const string launcher::IsotropicElasticMaterialLoader::RHEOLOGY_TYPE = "IsotropicElastic";

gcm::IsotropicElasticMaterial* launcher::IsotropicElasticMaterialLoader::load(xml::Node desc)
{
        string name = desc["name"];
        string rheology = desc["rheology"];
        assert(rheology == RHEOLOGY_TYPE);

        gcm_real la = lexical_cast<gcm_real>(desc.getChildByName("la").getTextContent());
        gcm_real mu = lexical_cast<gcm_real>(desc.getChildByName("mu").getTextContent());
        gcm_real rho = lexical_cast<gcm_real>(desc.getChildByName("rho").getTextContent());
        gcm_real crackThreshold = lexical_cast<gcm_real>(desc.getChildByName("crackThreshold").getTextContent());

        if (la <= 0.0 || mu <= 0.0 || rho <= 0.0 || crackThreshold <= 0.0)
            THROW_INVALID_INPUT("Seems xml snippet does not contain valid rheology parameters.");

        return new IsotropicElasticMaterial(name, rho, crackThreshold, la, mu);
}
