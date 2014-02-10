#include "mesh/tetr/Vtu2MeshLoader.h"

string gcm::Vtu2MeshLoader::getType(){
	return "vtu2";
}

gcm::Vtu2MeshLoader::Vtu2MeshLoader() {
	INIT_LOGGER("gcm.Vtu2MeshLoader");
}

gcm::Vtu2MeshLoader::~Vtu2MeshLoader() {
}

void gcm::Vtu2MeshLoader::loadMesh(Params params, TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher)
{
	if (params.find(PARAM_FILE) == params.end()) {
		delete mesh; 
		THROW_INVALID_ARG("Vtu file name was not provided");
	}
	Vtu2TetrFileReader* reader = new Vtu2TetrFileReader();
	reader->readFile(engine->getFileLookupService().lookupFile(params[PARAM_FILE]), 
						mesh, dispatcher, mesh->getBody()->getEngine()->getRank());
	delete reader;
	
	mesh->preProcess();
}

void gcm::Vtu2MeshLoader::preLoadMesh(Params params, AABB* scene, int& sliceDirection, int& numberOfNodes) {
	if (params.find(PARAM_FILE) == params.end()) {
		THROW_INVALID_ARG("Vtu file name was not provided");
	}
	Vtu2TetrFileReader* reader = new Vtu2TetrFileReader();
	reader->preReadFile(engine->getFileLookupService().lookupFile(params[PARAM_FILE]), scene, sliceDirection, numberOfNodes);
	delete reader;
}
