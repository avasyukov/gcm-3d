#ifndef GCM_RECTANGULARCUT_CUBIC_MESH_GENERATOR_H_
#define GCM_RECTANGULARCUT_CUBIC_MESH_GENERATOR_H_

#include <map>
#include <string>

#include "libgcm/mesh/cube/RectangularCutCubicMesh.hpp"
#include "libgcm/Exception.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/util/Singleton.hpp"
#include "libgcm/GCMDispatcher.hpp"


namespace gcm {
    class RectangularCutCubicMeshGenerator: public Singleton<RectangularCutCubicMeshGenerator>
    {
    protected:
         USE_LOGGER;
    public:
        RectangularCutCubicMeshGenerator();
        ~RectangularCutCubicMeshGenerator();
        void loadMesh(RectangularCutCubicMesh* mesh, GCMDispatcher* dispatcher, float h,
		              int numX, int numY, int numZ,
					  int minX, int minY, int minZ,
					  int maxX, int maxY, int maxZ);
        void preLoadMesh(AABB* scene, int& sliceDirection, int& numberOfNodes, 
		                 float h, int numX, int numY, int numZ);
    };
}

#endif /* GCM_RECTANGULARCUT_CUBIC_MESH_GENERATOR_H_ */
