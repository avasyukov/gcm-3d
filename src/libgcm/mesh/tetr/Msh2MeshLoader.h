#ifndef GCM_MSH2_MESH_LOADER_H_
#define GCM_MSH2_MESH_LOADER_H_

#include <map>
#include <string>

#include "mesh/MeshLoader.h"
#include "mesh/tetr/TetrMeshSecondOrder.h"
#include "snapshot/VTK2SnapshotWriter.h"
#include "util/formats/Vtu2TetrFileReader.h"
#include "Exception.h"
#include "Utils.h"
#include "Logging.h"
#include "DummyDispatcher.h"

using namespace gcm;
using namespace std;

namespace gcm {
    class Msh2MeshLoader: public TemplatedMeshLoader<TetrMeshSecondOrder>
    {
    protected:
        /*
         * Loads mesh from using passed configuration
         */
         void loadMesh(Params params, TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher);
         void preLoadMesh(Params params, AABB* scene, int& sliceDirection, int& numberOfNodes);
         USE_LOGGER;
         string getVtkFileName(string mshFile);
    public:
        Msh2MeshLoader();
        ~Msh2MeshLoader();
        /*
         * Returns mesh loader type
         */
        string getType();
        void cleanUp();
    };
}

#endif /* GCM_MSH2_MESH_LOADER_H_ */
