#ifndef GCM_MSH_MESH_LOADER_H_
#define GCM_MSH_MESH_LOADER_H_

#include <map>
#include <string>

#include "MeshLoader.h"
#include "TetrMeshFirstOrder.h"
#include "../Exception.h"
#include "../Utils.h"
#include "../Logging.h"
#include "../util/formats/MshTetrFileReader.h"

using namespace gcm;
using namespace std;

namespace gcm {
	class MshMeshLoader: public TemplatedMeshLoader<TetrMeshFirstOrder>
	{
	protected:
		/*
		 * Loads mesh from using passed configuration
		 */
		 void loadMesh(Params params, TetrMeshFirstOrder* mesh, GCMDispatcher* dispatcher);
		 void preLoadMesh(Params params, AABB* scene);
		 USE_LOGGER;
	public:
		MshMeshLoader();
		~MshMeshLoader();
		/*
		 * Returns mesh loader type
		 */
		string getType();
	};
}

#endif /* GCM_MSH_MESH_LOADER_H_ */