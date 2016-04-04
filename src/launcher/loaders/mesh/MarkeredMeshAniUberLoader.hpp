#ifndef LAUNCHER_MARKEREDMESHANIUBERLOADER_H
#define LAUNCHER_MARKEREDMESHANIUBERLOADER_H

#include <string>

#include "libgcm/GCMDispatcher.hpp"
#include "libgcm/util/AABB.hpp"
#include "libgcm/util/Singleton.hpp"
#include "libgcm/mesh/euler/markers/MarkeredMesh.hpp"

#include "launcher/util/xml.hpp"

#include "launcher/loaders/mesh/MeshLoader.hpp"


namespace launcher
{
    class MarkeredMeshAniUberLoader: public gcm::Singleton<launcher::MarkeredMeshAniUberLoader>, public launcher::MeshLoader<gcm::MarkeredMesh>
    {
        protected:
            void parseDesc(const xml::Node& desc, std::string& id, gcm::vector3u& cellsNum, gcm::vector3r& cellSize, std::string& source);
            void loadMesh(const xml::Node& desc, gcm::MarkeredMesh* mesh) override;
        public:
            void preLoadMesh(const xml::Node& desc, gcm::AABB& aabb, int& sliceDirection, int& numberOfNodes);

            const static std::string MESH_TYPE;
    };
}
        
#endif
