#ifndef MARKERED_BOX_MESH_LOADER_H_
#define MARKERED_BOX_MESH_LOADER_H_

#include <string>

#include "libgcm/util/Singleton.hpp"
#include "libgcm/mesh/markers/MarkeredMesh.hpp"
#include "libgcm/mesh/Mesh.hpp"

using namespace std;

namespace gcm {
    class MarkeredBoxMeshGenerator: public Singleton<MarkeredBoxMeshGenerator> {
    public:
        ~MarkeredBoxMeshGenerator();
        MarkeredBoxMeshGenerator();
    
        void loadMesh(MarkeredMesh* mesh, GCMDispatcher* dispatcher, float meshEdge, float cubeEdge, int cellsNum);
        void preLoadMesh(AABB* scene, int& sliceDirection, int& numberOfNodes, float meshEdge, float cubeEdge, int cellsNum);
    };
}

#endif
