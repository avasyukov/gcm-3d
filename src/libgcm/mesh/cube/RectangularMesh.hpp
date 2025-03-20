#pragma once

#include "libgcm/mesh/cube/BasicCubicMesh.hpp"

namespace gcm
{
    // вынужденное наследование от базиккубикмеш, 
    // потому что нет времени писать свое
    class RectangularMesh: public BasicCubicMesh {

    public:
        RectangularMesh();
        RectangularMesh(std::string _type);
        ~RectangularMesh();

    };
}

