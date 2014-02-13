#ifndef GCM_GEO2_MESH_LOADER_H_
#define GCM_GEO2_MESH_LOADER_H_

#include <map>
#include <string>

#include "mesh/MeshLoader.h"
#include "mesh/tetr/TetrMeshSecondOrder.h"
#include "util/formats/MshTetrFileReader.h"
#include "util/formats/Vtu2TetrFileReader.h"
#include "snapshot/VTK2SnapshotWriter.h"
#include "Exception.h"
#include "Utils.h"
#include "Logging.h"
#include "DummyDispatcher.h"



#include <gmsh/Gmsh.h>
#include <gmsh/GModel.h>
#include <gmsh/GEntity.h>
#include <gmsh/MElement.h>
#include <gmsh/MVertex.h>
#include <gmsh/MTriangle.h>
#include <gmsh/MTetrahedron.h>

using namespace gcm;
using namespace std;

namespace gcm {
	class Geo2MeshLoader: public TemplatedMeshLoader<TetrMeshSecondOrder>
	{
	protected:
		/*
		 * Loads mesh from using passed configuration
		 */
		 void loadMesh(Params params, TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher);
		 void preLoadMesh(Params params, AABB* scene, int& sliceDirection, int& numberOfNodes);
		 void createMshFile(Params params);
		 USE_LOGGER;
		 bool isMshFileCreated(Params params);
		 string getMshFileName(string geoFile);
		 string getVtkFileName(string geoFile);
		 map<string,bool> createdFiles;
	public:
		Geo2MeshLoader();
		~Geo2MeshLoader();
		/*
		 * Returns mesh loader type
		 */
		string getType();
		void cleanUp();
	};
}

#endif /* GCM_GEO2_MESH_LOADER_H_ */
