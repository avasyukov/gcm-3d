#ifndef MARKERED_BOX_MESH_LOADER_H_
#define MARKERED_BOX_MESH_LOADER_H_

#include "libgcm/mesh/MeshLoader.hpp"

#include <string>

#include "libgcm/mesh/markers/MarkeredMesh.hpp"
#include "libgcm/mesh/Mesh.hpp"

using namespace std;

namespace gcm {
    class MarkeredBoxMeshGenerator: public TemplatedMeshLoader<MarkeredMesh> {
    protected:
        void loadMesh(Params params, MarkeredMesh* mesh, GCMDispatcher* dispatcher);
    public:
        ~MarkeredBoxMeshGenerator();
        MarkeredBoxMeshGenerator();
        string getType();
        void preLoadMesh(Params params, AABB* scene, int& sliceDirection, int& numberOfNodes);

        const static string PARAM_MESH_EDGE;
        const static string PARAM_CUBE_EDGE;
        const static string PARAM_CELL_NUM;
    };
}

#endif
