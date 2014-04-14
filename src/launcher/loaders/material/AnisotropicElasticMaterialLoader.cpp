#include "launcher/loaders/material/AnisotropicElasticMaterialLoader.hpp"

#include <cstdlib>
#include <cerrno>
#include <utility>
#include <sstream>
#include <boost/lexical_cast.hpp>

#include "libgcm/materials/IAnisotropicElasticMaterial.hpp"
#include "libgcm/util/AnisotropicMatrix3D.hpp"
#include "libgcm/util/AnisotropicMatrix3DAnalytical.hpp"
#include "libgcm/Exception.hpp"

using boost::lexical_cast;

const string launcher::AnisotropicElasticMaterialLoader::RHEOLOGY_TYPE = "AnisotropicElastic";
const string launcher::AnisotropicElasticMaterialLoader::MATRIX_TYPE_ANALYTICAL = "analytical";
const string launcher::AnisotropicElasticMaterialLoader::MATRIX_TYPE_NUMERICAL = "numerical";
          
launcher::AnisotropicElasticMaterialLoader::AnisotropicElasticMaterialLoader(): AnisotropicElasticMaterialLoader(MATRIX_TYPE_NUMERICAL)
{
}

launcher::AnisotropicElasticMaterialLoader::AnisotropicElasticMaterialLoader(string type): type(type)
{
}

gcm::AnisotropicElasticMaterial* launcher::AnisotropicElasticMaterialLoader::load(xml::Node desc)
{
    string name = desc["name"];
    string rheology = desc["rheology"];
    assert(rheology == RHEOLOGY_TYPE);

    gcm_real rho;
    gcm_real crackThreshold;
    IAnisotropicElasticMaterial::RheologyParameters params;
    AnisotropicElasticMaterial* result = nullptr;

    auto createMaterial = [&]()
    {
        if (type == MATRIX_TYPE_NUMERICAL)
            result = new NumericalAnisotropicElasticMaterial(name, rho, crackThreshold, params);
        else if (type == MATRIX_TYPE_ANALYTICAL)
            result = new AnalyticalAnisotropicElasticMaterial(name, rho, crackThreshold, params);
        else
            THROW_UNSUPPORTED("Specified anisotropic matrix implementation\"" + type + "\" is not supported yet.");
    };


    if (desc.hasAttribute("source"))
    {
        string source = desc["source"];
        auto m = dynamic_cast<AnisotropicElasticMaterial*>(Engine::getInstance().getMaterial(source));
        if (!m)
            THROW_INVALID_ARG("Material " + source + " does not seem to be anisotropic elastic.");
        rho = m->getRho();
        crackThreshold = m->getCrackThreshold();
        params = m->getParameters();
    
        xml::Node rotate = desc.getChildByName("rotate");

        auto d2r = [](double x){ return M_PI*x/180; };
    
        double a1, a2, a3;

        a1 = d2r(lexical_cast<double>(rotate["a1"]));
        a2 = d2r(lexical_cast<double>(rotate["a2"]));
        a3 = d2r(lexical_cast<double>(rotate["a3"]));
        
        createMaterial();
        
        result->rotate(a1, a2, a3);
    }
    else
    {
        rho = lexical_cast<gcm_real>(desc.getChildByName("rho").getTextContent());
        crackThreshold = lexical_cast<gcm_real>(desc.getChildByName("crackThreshold").getTextContent());

        int k = 0;
        for (int i = 1; i <= 6; i++)
            for (int j = i; j <= 6; j++) {
                stringstream cxx;
                cxx << "c" << i << j;
                params.values[k++] = lexical_cast<gcm_real>(desc.getChildByName(cxx.str()).getTextContent());
            }

        if (rho <= 0.0)
            THROW_INVALID_INPUT("Seems xml snippet does not contain valid rheology parameters.");

        for (k = 0; k < ANISOTROPIC_ELASTIC_MATERIALS_PARAMETERS_NUM; k++)
            if (params.values[k] < 0.0)
                THROW_INVALID_INPUT("Seems xml snippet does not contain valid rheology parameters.");

        createMaterial();
    }


    return result;
}
