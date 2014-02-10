#ifndef GCM_GEO_MESH_LOADER_H_
#define GCM_GEO_MESH_LOADER_H_

#include <map>
#include <string>

#include <gmsh/Gmsh.h>
#include <gmsh/GModel.h>
#include <gmsh/GEntity.h>
#include <gmsh/MElement.h>
#include <gmsh/MVertex.h>
#include <gmsh/MTriangle.h>
#include <gmsh/MTetrahedron.h>

#include "mesh/MeshLoader.h"
#include "mesh/tetr/TetrMeshFirstOrder.h"
#include "util/formats/MshTetrFileReader.h"
#include "Exception.h"
#include "Utils.h"
#include "Logging.h"


using namespace gcm;
using namespace std;

namespace gcm {
	class GeoMeshLoader: public TemplatedMeshLoader<TetrMeshFirstOrder>
	{
	protected:
		/*
		 * Loads mesh from using passed configuration
		 */
		 void loadMesh(Params params, TetrMeshFirstOrder* mesh, GCMDispatcher* dispatcher);
		 void preLoadMesh(Params params, AABB* scene, int& sliceDirection, int& numberOfNodes);
		 void createMshFile(Params params);
		 USE_LOGGER;
		 bool isMshFileCreated(Params params);
		 string getMshFileName(string geoFile);
		 map<string,bool> createdFiles;
	public:
		GeoMeshLoader();
		~GeoMeshLoader();
		/*
		 * Returns mesh loader type
		 */
		string getType();
		void cleanUp();
	};
}

#endif /* GCM_GEO_MESH_LOADER_H_ */
