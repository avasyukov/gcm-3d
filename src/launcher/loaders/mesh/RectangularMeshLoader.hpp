#pragma once

#include <string>

#include "libgcm/GCMDispatcher.hpp"
#include "libgcm/util/AABB.hpp"
#include "libgcm/util/Singleton.hpp"
#include "libgcm/mesh/cube/RectangularMesh.hpp"

#include "launcher/util/xml.hpp"

#include "launcher/loaders/mesh/MeshLoader.hpp"

namespace launcher
{
    class RectangularMeshLoader: public gcm::Singleton<launcher::RectangularMeshLoader>, public launcher::MeshLoader<gcm::RectangularMesh>
    {
        protected:
            void parseDesc(const xml::Node& desc, std::string& id, int& numX, int& numY, 
                            int& numZ, double& sx, double& sy, double& sz);
            void loadMesh(const xml::Node& desc, gcm::RectangularMesh* mesh) override;
        public:
            void preLoadMesh(const xml::Node& desc, gcm::AABB& aabb, int& sliceDirection, int& numberOfNodes, 
                                double& sx, double& sy, double& sz);

            const static std::string MESH_TYPE;
    };
}
