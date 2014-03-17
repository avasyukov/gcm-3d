/*
 * File:   IsotropicElasticMaterialLoader.h
 * Author: Alexey Ermakov
 *
 * Created on March 8, 2014, 8:43 PM
 */

#ifndef ISOTROPICELASTICMATERIALLOADER_H
#define	ISOTROPICELASTICMATERIALLOADER_H

#include "materials/IsotropicElasticMaterial.h"
#include "util/Singleton.h"
#include "util/xml.h"

using namespace gcm;
using namespace xml;
using namespace std;

namespace launcher
{

    class IsotropicElasticMaterialLoader : public Singleton<IsotropicElasticMaterialLoader>
    {
    public:
        // FIXME possible memory leak here.
        IsotropicElasticMaterial* load(xml::Node desc);

        const static string RHEOLOGY_TYPE;

    };
};

#endif	/* ISOTROPICELASTICMATERIALLOADER_H */

