#ifndef GCM_VTU_MESH_LOADER_H_
#define GCM_VTU_MESH_LOADER_H_

#include <map>
#include <string>

#include "MeshLoader.h"
#include "TetrMeshFirstOrder.h"
#include "../Exception.h"
#include "../Utils.h"
#include "../Logging.h"
#include "../util/formats/VtuTetrFileReader.h"

using namespace gcm;
using namespace std;

namespace gcm {
	class VtuMeshLoader: public TemplatedMeshLoader<TetrMeshFirstOrder>
	{
	protected:
		/*
		 * Loads mesh from using passed configuration
		 */
		 void loadMesh(Params params, TetrMeshFirstOrder* mesh, GCMDispatcher* dispatcher);
		 void preLoadMesh(Params params, AABB* scene, int& sliceDirection, int& numberOfNodes);
		 USE_LOGGER;
	public:
		VtuMeshLoader();
		~VtuMeshLoader();
		/*
		 * Returns mesh loader type
		 */
		string getType();
	};
}

#endif /* GCM_VTU_MESH_LOADER_H_ */