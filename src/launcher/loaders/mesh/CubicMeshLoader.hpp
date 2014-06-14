#ifndef LAUNCHER_CUBICMESHLOADER_H
#define LAUNCHER_CUBICMESHLOADER_H

#include <string>

#include "libgcm/GCMDispatcher.hpp"
#include "libgcm/util/AABB.hpp"
#include "libgcm/util/Singleton.hpp"
#include "libgcm/mesh/cube/BasicCubicMesh.hpp"

#include "launcher/util/xml.hpp"

#include "launcher/loaders/mesh/MeshLoader.hpp"

using namespace gcm;
using namespace std;

namespace launcher
{
    class CubicMeshLoader: public Singleton<launcher::CubicMeshLoader>, public launcher::MeshLoader<BasicCubicMesh>
    {
        protected:
            void parseDesc(const xml::Node& desc, string& id, float& h, int& num);
            void loadMesh(const xml::Node& desc, BasicCubicMesh* mesh) override;
        public:
            void preLoadMesh(const xml::Node& desc, AABB& aabb, int& sliceDirection, int& numberOfNodes);

            const static string MESH_TYPE;
    };
}
        
#endif
