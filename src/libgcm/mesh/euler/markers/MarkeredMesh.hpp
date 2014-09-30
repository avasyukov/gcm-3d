#ifndef MARKEREDMESH_HPP_
#define MARKEREDMESH_HPP_


#include "libgcm/mesh/euler/EulerMesh.hpp"
#include "libgcm/mesh/euler/markers/MarkeredSurface.hpp"

#include <unordered_map>
#include <vector>

namespace gcm
{
    class MarkeredMesh: public EulerMesh
    {
     protected:
        /**
         * Markered surface.
         */
        MarkeredSurface surface;
     public:
        MarkeredMesh();
        MarkeredMesh(MarkeredSurface surface, vector3u dimensions, vector3r cellSize, vector3r center = {0, 0, 0});
        void preProcessGeometry() override;
        void checkTopology(float tau) override;
        void logMeshStats() override;

        void setSurface(const MarkeredSurface& surface);
        void reconstructBorder();

        void transfer(float x, float y, float z) override;
        void moveCoords(float tau) override;


    };
}

#endif
