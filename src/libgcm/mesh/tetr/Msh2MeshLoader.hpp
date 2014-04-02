#ifndef GCM_MSH2_MESH_LOADER_H_
#define GCM_MSH2_MESH_LOADER_H_

#include <map>
#include <string>

#include "libgcm/mesh/tetr/TetrMeshSecondOrder.hpp"
#include "libgcm/snapshot/VTK2SnapshotWriter.hpp"
#include "libgcm/util/formats/Vtu2TetrFileReader.hpp"
#include "libgcm/Exception.hpp"
#include "libgcm/Utils.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/util/Singleton.hpp"
#include "libgcm/DummyDispatcher.hpp"

using namespace gcm;
using namespace std;

namespace gcm {
    class Msh2MeshLoader: public Singleton<Msh2MeshLoader>
    {
    protected:
         USE_LOGGER;
         string getVtkFileName(string mshFile);
    public:
        Msh2MeshLoader();
        ~Msh2MeshLoader();
         
        void loadMesh(TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher, const string& fileName);
        void preLoadMesh(AABB* scene, int& sliceDirection, int& numberOfNodes, const string& fileName);
       
       void cleanUp();
    };
}

#endif /* GCM_MSH2_MESH_LOADER_H_ */
