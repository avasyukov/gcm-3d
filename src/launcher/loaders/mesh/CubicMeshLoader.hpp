#ifndef LAUNCHER_CUBICMESHLOADER_H
#define LAUNCHER_CUBICMESHLOADER_H

#include <string>

#include "libgcm/GCMDispatcher.hpp"
#include "libgcm/util/AABB.hpp"
#include "libgcm/util/Singleton.hpp"
#include "libgcm/mesh/cube/BasicCubicMesh.hpp"

#include "launcher/util/xml.hpp"

#include "launcher/loaders/mesh/MeshLoader.hpp"


namespace launcher
{
    class CubicMeshLoader: public gcm::Singleton<launcher::CubicMeshLoader>, public launcher::MeshLoader<gcm::BasicCubicMesh>
    {
        protected:
            void parseDesc(const xml::Node& desc, std::string& id, float& h, int& num);
            void loadMesh(const xml::Node& desc, gcm::BasicCubicMesh* mesh) override;
        public:
            void preLoadMesh(const xml::Node& desc, gcm::AABB& aabb, int& sliceDirection, int& numberOfNodes);

            const static std::string MESH_TYPE;
    };
}
        
#endif
