#pragma once

#include "libgcm/mesh/cube/BasicCubicMesh.hpp"

namespace gcm
{
    class RectangularMesh: public BasicCubicMesh {

    public:
        RectangularMesh();
        RectangularMesh(std::string _type);
        ~RectangularMesh();

    };
}

