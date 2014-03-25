/*
 * File:   IsotropicElasticMaterialLoader.h
 * Author: Alexey Ermakov
 *
 * Created on March 8, 2014, 8:43 PM
 */

#ifndef ANISOTROPICELASTICMATERIALLOADER_H
#define    ANISOTROPICELASTICMATERIALLOADER_H

#include "libgcm/materials/AnisotropicElasticMaterial.h"
#include "libgcm/util/Singleton.h"
#include "launcher/util/xml.h"

using namespace gcm;
using namespace xml;
using namespace std;

namespace launcher {
    class AnisotropicElasticMaterialLoader: public Singleton<AnisotropicElasticMaterialLoader> {
    public:
          // FIXME possible memory leak here.
          AnisotropicElasticMaterial* load(xml::Node desc);

          const static string RHEOLOGY_TYPE;

    };
};

#endif    /* ANISOTROPICELASTICMATERIALLOADER_H */

