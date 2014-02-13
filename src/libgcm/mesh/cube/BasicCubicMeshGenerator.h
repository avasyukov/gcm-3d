#ifndef GCM_BASIC_CUBUC_MESH_GENERATOR_H_
#define GCM_BASIC_CUBUC_MESH_GENERATOR_H_

#include <map>
#include <string>

#include "mesh/MeshLoader.h"
#include "mesh/cube/BasicCubicMesh.h"
#include "Exception.h"
#include "Utils.h"
#include "Logging.h"

using namespace gcm;
using namespace std;

namespace gcm {
	class BasicCubicMeshGenerator: public TemplatedMeshLoader<BasicCubicMesh>
	{
	protected:
		/*
		 * Loads mesh from using passed configuration
		 */
		 void loadMesh(Params params, BasicCubicMesh* mesh, GCMDispatcher* dispatcher);
		 void preLoadMesh(Params params, AABB* scene, int& sliceDirection, int& numberOfNodes);
		 USE_LOGGER;
		 void checkParams(Params params);
	public:
		BasicCubicMeshGenerator();
		~BasicCubicMeshGenerator();
		/*
		 * Returns mesh loader type
		 */
		string getType();
		
		static const string PARAM_H;
		static const string PARAM_NUMBER;
	};
}

#endif /* GCM_BASIC_CUBUC_MESH_GENERATOR_H_ */
