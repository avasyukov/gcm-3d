#ifndef UNIFIEDMATERIALLOADER_HPP
#define UNIFIEDMATERIALLOADER_HPP 

#include <string>

#include "libgcm/util/Singleton.hpp"
#include "libgcm/rheology/Material.hpp"

#include "launcher/util/xml.hpp"

namespace launcher
{
    class MaterialLoader: public gcm::Singleton<MaterialLoader>
    {
        protected:
            gcm::MaterialPtr loadIsotropic(const xml::Node& desc, gcm::Material::PlasticityProperties plasticityProps);
            gcm::MaterialPtr loadAnisotropic(const xml::Node& desc, gcm::Material::PlasticityProperties plasticityProps);
        public:
            MaterialLoader();
          
            gcm::MaterialPtr load(const xml::Node& desc);

            const static std::string MATERIAL_TYPE_ISOTROPIC;
            const static std::string MATERIAL_TYPE_ANISOTROPIC;
    };
}

#endif /* UNIFIEDMATERIALLOADER_HPP */
