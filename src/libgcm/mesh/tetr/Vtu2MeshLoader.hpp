#ifndef GCM_VTU2_MESH_LOADER_H_
#define GCM_VTU2_MESH_LOADER_H_

#include <map>
#include <string>

#include "libgcm/mesh/tetr/TetrMeshSecondOrder.hpp"
#include "libgcm/util/formats/Vtu2TetrFileReader.hpp"
#include "libgcm/Exception.hpp"
#include "libgcm/util/Singleton.hpp"
#include "libgcm/Logging.hpp"

using namespace gcm;
using namespace std;

namespace gcm {
    class Vtu2MeshLoader: public Singleton<Vtu2MeshLoader>
    {
    protected:
         USE_LOGGER;
    public:
        Vtu2MeshLoader();
        ~Vtu2MeshLoader();
        void loadMesh(TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher, const string& fileName);
        void preLoadMesh(AABB* scene, int& sliceDirection, int& numberOfNodes, const string& fileName);
    };
}

#endif /* GCM_VTU2_MESH_LOADER_H_ */
