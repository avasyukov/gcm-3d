#ifndef LAUNCHER_MSH2MESHLOADER_H
#define LAUNCHER_MSH2MESHLOADER_H

#include <string>

#include "libgcm/GCMDispatcher.hpp"
#include "libgcm/util/AABB.hpp"
#include "libgcm/util/Singleton.hpp"
#include "libgcm/mesh/tetr/TetrMeshSecondOrder.hpp"
#include "libgcm/mesh/tetr/Msh2MeshLoader.hpp"

#include "launcher/util/xml.hpp"

#include "launcher/loaders/mesh/MeshLoader.hpp"


namespace launcher
{
    class Msh2MeshLoader: public gcm::Singleton<launcher::Msh2MeshLoader>, public launcher::MeshLoader<gcm::TetrMeshSecondOrder>
    {
        protected:
            void parseDesc(const xml::Node& desc, std::string& id, std::string& fileName);
            void loadMesh(const xml::Node& desc, gcm::TetrMeshSecondOrder* mesh) override;
        public:
            void preLoadMesh(const xml::Node& desc, gcm::AABB& aabb, int& sliceDirection, int& numberOfNodes);

            const static std::string MESH_TYPE;
    };
}
        
#endif
