/*
 * File:   IsotropicElasticMaterialLoader.h
 * Author: Alexey Ermakov
 *
 * Created on March 8, 2014, 8:43 PM
 */

#ifndef ANISOTROPICELASTICMATERIALLOADER_H
#define    ANISOTROPICELASTICMATERIALLOADER_H

#include "libgcm/materials/AnisotropicElasticMaterial.hpp"
#include "libgcm/util/Singleton.hpp"
#include "launcher/util/xml.hpp"

using namespace gcm;
using namespace xml;
using namespace std;

namespace launcher {
    class AnisotropicElasticMaterialLoader: public Singleton<AnisotropicElasticMaterialLoader> {
    public:
          // FIXME possible memory leak here.
          AnisotropicElasticMaterial* load(xml::Node desc);
          AnisotropicElasticMaterial* load(xml::Node desc, string anisotropicMatrixImplementation);

          const static string RHEOLOGY_TYPE;

    };
};

#endif    /* ANISOTROPICELASTICMATERIALLOADER_H */

