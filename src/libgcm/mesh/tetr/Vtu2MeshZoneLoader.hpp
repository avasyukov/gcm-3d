#ifndef GCM_VTU2_MESH_ZONE_LOADER_H_
#define GCM_VTU2_MESH_ZONE_LOADER_H_

#include <map>
#include <string>

#include "libgcm/mesh/tetr/TetrMeshSecondOrder.hpp"
#include "libgcm/mesh/MeshLoader.hpp"
#include "libgcm/util/formats/Vtu2TetrFileReader.hpp"
#include "libgcm/Exception.hpp"
#include "libgcm/util/Singleton.hpp"
#include "libgcm/Logging.hpp"


using namespace gcm;
using namespace std;

namespace gcm {
    class Vtu2MeshZoneLoader: public Singleton<Vtu2MeshZoneLoader>
    {
    protected:
         USE_LOGGER;
    public:
        Vtu2MeshZoneLoader();
        ~Vtu2MeshZoneLoader();
         void loadMesh(TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher, const string& fileName);
         void preLoadMesh(AABB* scene, int& sliceDirection, int& numberOfNodes, const string& fileName);
    };
}

#endif /* GCM_VTU2_MESH_ZONE_LOADER_H_ */
