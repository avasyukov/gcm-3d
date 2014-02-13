#include "mesh/tetr/MshMeshLoader.h"

string gcm::MshMeshLoader::getType(){
	return "msh";
}

gcm::MshMeshLoader::MshMeshLoader() {
	INIT_LOGGER("gcm.MshMeshLoader");
}

gcm::MshMeshLoader::~MshMeshLoader() {
}

void gcm::MshMeshLoader::loadMesh(Params params, TetrMeshFirstOrder* mesh, GCMDispatcher* dispatcher)
{
	THROW_INVALID_ARG("First order mesh is not supported now. Please use second order mesh and msh2 loader.");
	if (params.find(PARAM_FILE) == params.end()) {
		delete mesh; 
		THROW_INVALID_ARG("Msh file name was not provided");
	}
	MshTetrFileReader* reader = new MshTetrFileReader();
	reader->readFile(engine->getFileLookupService().lookupFile(params[PARAM_FILE]), 
							mesh, dispatcher, mesh->getBody()->getEngine()->getRank(), false);
	delete reader;
	
	mesh->preProcess();
}

void gcm::MshMeshLoader::preLoadMesh(Params params, AABB* scene, int& sliceDirection, int& numberOfNodes) {
	if (params.find(PARAM_FILE) == params.end()) {
		THROW_INVALID_ARG("Msh file name was not provided");
	}
	MshTetrFileReader* reader = new MshTetrFileReader();
	reader->preReadFile(engine->getFileLookupService().lookupFile(params[PARAM_FILE]), scene, sliceDirection, numberOfNodes);
	delete reader;
}
