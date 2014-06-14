#ifndef LAUNCHER_MARKEREDBOXMESHLOADER_H
#define LAUNCHER_MARKEREDBOXMESHLOADER_H

#include <string>

#include "libgcm/GCMDispatcher.hpp"
#include "libgcm/util/AABB.hpp"
#include "libgcm/util/Singleton.hpp"
#include "libgcm/mesh/markers/MarkeredMesh.hpp"

#include "launcher/util/xml.hpp"

#include "launcher/loaders/mesh/MeshLoader.hpp"

using namespace gcm;
using namespace std;

namespace launcher
{
    class MarkeredBoxMeshLoader: public Singleton<launcher::MarkeredBoxMeshLoader>, public launcher::MeshLoader<MarkeredMesh>
    {
        protected:
            void parseDesc(const xml::Node& desc, string& id, float& cubeEdge, float& meshEdge, int& cellNum);
            void loadMesh(const xml::Node& desc, MarkeredMesh* mesh) override;
        public:
            void preLoadMesh(const xml::Node& desc, AABB& aabb, int& sliceDirection, int& numberOfNodes);

            const static string MESH_TYPE;
    };
}
        
#endif
