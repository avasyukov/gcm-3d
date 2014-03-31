#ifndef GCM_VTU2_MESH_LOADER_H_
#define GCM_VTU2_MESH_LOADER_H_

#include <map>
#include <string>

#include "libgcm/mesh/tetr/TetrMeshSecondOrder.hpp"
#include "libgcm/mesh/MeshLoader.hpp"
#include "libgcm/util/formats/Vtu2TetrFileReader.hpp"
#include "libgcm/Exception.hpp"
#include "libgcm/Utils.hpp"
#include "libgcm/Logging.hpp"

using namespace gcm;
using namespace std;

namespace gcm {
    class Vtu2MeshLoader: public TemplatedMeshLoader<TetrMeshSecondOrder>
    {
    protected:
        /*
         * Loads mesh from using passed configuration
         */
         void loadMesh(Params params, TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher);
         void preLoadMesh(Params params, AABB* scene, int& sliceDirection, int& numberOfNodes);
         USE_LOGGER;
    public:
        Vtu2MeshLoader();
        ~Vtu2MeshLoader();
        /*
         * Returns mesh loader type
         */
        string getType();
    };
}

#endif /* GCM_VTU_MESH_LOADER_H_ */
