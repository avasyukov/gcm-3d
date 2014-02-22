#ifndef MARKERED_BOX_MESH_LOADER_H_
#define MARKERED_BOX_MESH_LOADER_H_

#include "mesh/MeshLoader.h"

#include <string>

#include "mesh/markers/MarkeredMesh.h"
#include "mesh/Mesh.h"

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
