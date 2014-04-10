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
    protected:
        string type;
    public:
          AnisotropicElasticMaterialLoader();
          AnisotropicElasticMaterialLoader(string type);

          // FIXME possible memory leak here.
          AnisotropicElasticMaterial* load(xml::Node desc);

          const static string RHEOLOGY_TYPE;
          const static string MATRIX_TYPE_ANALYTICAL;
          const static string MATRIX_TYPE_NUMERICAL;

    };
};

#endif    /* ANISOTROPICELASTICMATERIALLOADER_H */


