#pragma once

#include <map>
#include <string>

#include "libgcm/mesh/cube/RectangularMesh.hpp"
#include "libgcm/Exception.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/util/Singleton.hpp"
#include "libgcm/GCMDispatcher.hpp"


namespace gcm {
    class RectangularMeshGenerator: public Singleton<RectangularMeshGenerator>
    {
    protected:
         USE_LOGGER;
    public:
        RectangularMeshGenerator();
        ~RectangularMeshGenerator();

        void loadMesh(RectangularMesh* mesh, GCMDispatcher* dispatcher,
		               int numX, int numY, int numZ, double& sx, double& sy, double& sz);

        void preLoadMesh(AABB* scene, int& sliceDirection, int& numberOfNodes, 
		                  int numX, int numY, int numZ, double& sx, double& sy, double& sz);
    };
}

