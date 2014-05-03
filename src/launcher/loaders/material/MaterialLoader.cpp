#include "launcher/loaders/material/MaterialLoader.hpp"

#include "libgcm/rheology/Material.hpp"
#include "libgcm/Exception.hpp"
#include "libgcm/Engine.hpp"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace gcm;
using boost::lexical_cast;

const string launcher::MaterialLoader::MATERIAL_TYPE_ISOTROPIC = "isotropic";
const string launcher::MaterialLoader::MATERIAL_TYPE_ANISOTROPIC = "anisotropic";

launcher::MaterialLoader::MaterialLoader()
{
}
          
MaterialPtr launcher::MaterialLoader::loadIsotropic(const xml::Node& desc, Material::PlasticityProperties plasticityProps)
{
    string name = desc["name"];
    
    gcm_real rho = lexical_cast<gcm_real>(desc.getChildByName("rho").getTextContent());
    gcm_real crackThreshold = lexical_cast<gcm_real>(desc.getChildByName("crackThreshold").getTextContent());
    gcm_real la = lexical_cast<gcm_real>(desc.getChildByName("la").getTextContent());
    gcm_real mu = lexical_cast<gcm_real>(desc.getChildByName("mu").getTextContent());

    if (la <= 0.0 || mu <= 0.0 || rho <= 0.0 || crackThreshold <= 0.0)
        THROW_INVALID_INPUT("Seems xml snippet does not contain valid rheology parameters.");
    
    return makeMaterialPtr(name, rho, crackThreshold, la, mu, plasticityProps);
}

MaterialPtr launcher::MaterialLoader::loadAnisotropic(const xml::Node& desc, Material::PlasticityProperties plasticityProps)
{
    string name = desc["name"];

    if (desc.hasAttribute("source"))
    {
        string source = desc["source"];
        auto& m = Engine::getInstance().getMaterial(source);
    
        xml::Node rotate = desc.getChildByName("rotate");

        auto d2r = [](double x){ return M_PI*x/180; };
    
        double a1, a2, a3;

        a1 = d2r(lexical_cast<double>(rotate["a1"]));
        a2 = d2r(lexical_cast<double>(rotate["a2"]));
        a3 = d2r(lexical_cast<double>(rotate["a3"]));
        
        return makeMaterialPtr(m, name, a1, a2, a3);
    }
    else
    {
        gcm_real rho = lexical_cast<gcm_real>(desc.getChildByName("rho").getTextContent());
        gcm_real crackThreshold = lexical_cast<gcm_real>(desc.getChildByName("crackThreshold").getTextContent());

        Material::RheologyProperties props;

        int k = 0;
        for (int i = 1; i <= 6; i++)
            for (int j = i; j <= 6; j++) {
                stringstream cxx;
                cxx << "c" << i << j;
                props.values[k++] = lexical_cast<gcm_real>(desc.getChildByName(cxx.str()).getTextContent());
            }

        if (rho <= 0.0)
            THROW_INVALID_INPUT("Seems xml snippet does not contain valid rheology parameters.");

        for (auto v: props.values)
            if (v < 0.0)
                THROW_INVALID_INPUT("Seems xml snippet does not contain valid rheology parameters.");

        return makeMaterialPtr(name, rho, crackThreshold, props, plasticityProps);
    }
}

MaterialPtr launcher::MaterialLoader::load(const xml::Node& desc)
{
    string rheology = desc["rheology"];

    Material::PlasticityProperties plasticityProps;

    for (auto p: desc.getChildrenByName("plasticity"))
    {
        string type = p["type"];
        for (auto pp: p.getChildNodes())
            plasticityProps[type][pp.getTagName()] = lexical_cast<float>(pp.getTextContent());
    }

    if (rheology == MATERIAL_TYPE_ISOTROPIC)
        return loadIsotropic(desc, plasticityProps);
    else if (rheology == MATERIAL_TYPE_ANISOTROPIC)
        return loadAnisotropic(desc, plasticityProps);
    else
        THROW_UNSUPPORTED("Material type\"" + rheology + "\" is not supported");
}
