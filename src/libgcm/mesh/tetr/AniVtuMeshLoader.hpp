#ifndef GCM_ANI_VTU_MESH_LOADER_H_
#define GCM_ANI_VTU_MESH_LOADER_H_

#include <map>
#include <string>

#include "libgcm/mesh/tetr/TetrMeshSecondOrder.hpp"
#include "libgcm/util/formats/AniVtuTetrFileReader.hpp"
#include "libgcm/Exception.hpp"
#include "libgcm/util/Singleton.hpp"
#include "libgcm/Logging.hpp"


namespace gcm {
    class AniVtuMeshLoader: public Singleton<AniVtuMeshLoader>
    {
    protected:
         USE_LOGGER;
    public:
        AniVtuMeshLoader();
        ~AniVtuMeshLoader();
        void loadMesh(TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher, const std::string& fileName);
        void preLoadMesh(AABB* scene, int& sliceDirection, int& numberOfNodes, const std::string& fileName);
    };
}

#endif /* GCM_ANI_VTU_MESH_LOADER_H_ */
