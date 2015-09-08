#ifndef GCM_BASIC_CUBIC_MESH_GENERATOR_H_
#define GCM_BASIC_CUBIC_MESH_GENERATOR_H_

#include <map>
#include <string>

#include "libgcm/mesh/cube/BasicCubicMesh.hpp"
#include "libgcm/Exception.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/util/Singleton.hpp"
#include "libgcm/GCMDispatcher.hpp"


namespace gcm {
    class BasicCubicMeshGenerator: public Singleton<BasicCubicMeshGenerator>
    {
    protected:
         USE_LOGGER;
    public:
        BasicCubicMeshGenerator();
        ~BasicCubicMeshGenerator();
        void loadMesh(BasicCubicMesh* mesh, GCMDispatcher* dispatcher, float h,
		              int numX, int numY, int numZ);
        void preLoadMesh(AABB* scene, int& sliceDirection, int& numberOfNodes, 
		                 float h, int numX, int numY, int numZ);
    };
}

#endif /* GCM_BASIC_CUBIC_MESH_GENERATOR_H_ */
