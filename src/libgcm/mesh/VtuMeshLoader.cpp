#include "VtuMeshLoader.h"

string gcm::VtuMeshLoader::getType(){
	return "vtu";
}

gcm::VtuMeshLoader::VtuMeshLoader() {
	INIT_LOGGER("gcm.VtuMeshLoader");
}

gcm::VtuMeshLoader::~VtuMeshLoader() {
}

void gcm::VtuMeshLoader::loadMesh(Params params, TetrMeshFirstOrder* mesh, GCMDispatcher* dispatcher)
{
	THROW_INVALID_ARG("First order mesh is not supported now. Please use second order mesh and vtu2 loader.");
	if (params.find(PARAM_FILE) == params.end()) {
		delete mesh; 
		THROW_INVALID_ARG("Vtu file name was not provided");
	}
	VtuTetrFileReader* reader = new VtuTetrFileReader();
	reader->readFile(engine->getFileLookupService().lookupFile(params[PARAM_FILE]), 
						mesh, dispatcher, mesh->getBody()->getEngine()->getRank());
	delete reader;
	
	mesh->preProcess();
}

void gcm::VtuMeshLoader::preLoadMesh(Params params, AABB* scene, int& sliceDirection, int& numberOfNodes) {
	if (params.find(PARAM_FILE) == params.end()) {
		THROW_INVALID_ARG("Vtu file name was not provided");
	}
	VtuTetrFileReader* reader = new VtuTetrFileReader();
	reader->preReadFile(engine->getFileLookupService().lookupFile(params[PARAM_FILE]), scene, sliceDirection, numberOfNodes);
	delete reader;
}