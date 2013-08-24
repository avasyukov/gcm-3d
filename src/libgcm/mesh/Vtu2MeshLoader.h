#ifndef GCM_VTU2_MESH_LOADER_H_
#define GCM_VTU2_MESH_LOADER_H_

#include <map>
#include <string>

#include "MeshLoader.h"
#include "TetrMeshSecondOrder.h"
#include "../Exception.h"
#include "../Utils.h"
#include "../Logging.h"
#include "../util/formats/Vtu2TetrFileReader.h"

using namespace gcm;
using namespace std;

namespace gcm {
	class Vtu2MeshLoader: public TemplatedMeshLoader<TetrMeshSecondOrder>
	{
	protected:
		/*
		 * Loads mesh from using passed configuration
		 */
		 void loadMesh(Params params, TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher);
		 void preLoadMesh(Params params, AABB* scene, int& sliceDirection, int& numberOfNodes);
		 USE_LOGGER;
	public:
		Vtu2MeshLoader();
		~Vtu2MeshLoader();
		/*
		 * Returns mesh loader type
		 */
		string getType();
	};
}

#endif /* GCM_VTU_MESH_LOADER_H_ */