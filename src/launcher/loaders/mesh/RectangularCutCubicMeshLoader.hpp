#ifndef LAUNCHER_RECTANGULARCUTCUBICMESHLOADER_H
#define LAUNCHER_RECTANGULARCUTCUBICMESHLOADER_H

#include <string>

#include "libgcm/GCMDispatcher.hpp"
#include "libgcm/util/AABB.hpp"
#include "libgcm/util/Singleton.hpp"
#include "libgcm/mesh/cube/RectangularCutCubicMesh.hpp"

#include "launcher/util/xml.hpp"

#include "launcher/loaders/mesh/MeshLoader.hpp"


namespace launcher
{
    class RectangularCutCubicMeshLoader: 
		public gcm::Singleton<launcher::RectangularCutCubicMeshLoader>, 
		public launcher::MeshLoader<gcm::RectangularCutCubicMesh>
    {
        protected:
            void parseDesc(const xml::Node& desc, std::string& id, float& h,
				int& numX, int& numY, int& numZ);
            void loadMesh(const xml::Node& desc, 
				gcm::RectangularCutCubicMesh* mesh) override;
        public:
            void preLoadMesh(const xml::Node& desc, gcm::AABB& aabb, 
				int& sliceDirection, int& numberOfNodes);

            const static std::string MESH_TYPE;
    };
}
        
#endif
