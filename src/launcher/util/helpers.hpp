//
// Created by lx on 02.10.15.
//

#ifndef GCM_3D_HELPERS_HPP
#define GCM_3D_HELPERS_HPP

#include "launcher/util/xml.hpp"
#include "libgcm/util/areas/Area.hpp"

namespace launcher
{
    gcm::Area* readCylinderArea(const xml::Node& areaNode);
    gcm::Area* readSphereArea(const xml::Node& areaNode);
    gcm::Area* readBoxArea(const xml::Node& areaNode);
    gcm::Area* readArea(const xml::Node& areaNode);
};

#endif //GCM_3D_HELPERS_HPP
