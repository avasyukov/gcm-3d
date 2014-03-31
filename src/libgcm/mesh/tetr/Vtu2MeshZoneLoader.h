#ifndef GCM_VTU2_MESH_ZONE_LOADER_H_
#define GCM_VTU2_MESH_ZONE_LOADER_H_

#include <map>
#include <string>

#include "libgcm/mesh/tetr/TetrMeshSecondOrder.h"
#include "libgcm/mesh/MeshLoader.h"
#include "libgcm/util/formats/Vtu2TetrFileReader.h"
#include "libgcm/Exception.h"
#include "libgcm/Utils.h"
#include "libgcm/Logging.h"


using namespace gcm;
using namespace std;

namespace gcm {
    class Vtu2MeshZoneLoader: public TemplatedMeshLoader<TetrMeshSecondOrder>
    {
    protected:
        /*
         * Loads mesh from using passed configuration
         */
         void loadMesh(Params params, TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher);
         void preLoadMesh(Params params, AABB* scene, int& sliceDirection, int& numberOfNodes);
         USE_LOGGER;
    public:
        Vtu2MeshZoneLoader();
        ~Vtu2MeshZoneLoader();
        /*
         * Returns mesh loader type
         */
        string getType();
    };
}

#endif /* GCM_VTU_MESH_ZONE_LOADER_H_ */
