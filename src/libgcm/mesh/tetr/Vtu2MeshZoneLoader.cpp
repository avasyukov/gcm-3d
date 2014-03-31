#include "libgcm/mesh/tetr/Vtu2MeshZoneLoader.h"

string gcm::Vtu2MeshZoneLoader::getType(){
    return "vtu2zone";
}

gcm::Vtu2MeshZoneLoader::Vtu2MeshZoneLoader() {
    INIT_LOGGER("gcm.Vtu2MeshZoneLoader");
}

gcm::Vtu2MeshZoneLoader::~Vtu2MeshZoneLoader() {
}

void gcm::Vtu2MeshZoneLoader::loadMesh(Params params, TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher)
{
    if (params.find(PARAM_FILE) == params.end()) {
        delete mesh;
        THROW_INVALID_ARG("Vtu file name was not provided");
    }
    Vtu2TetrFileReader* reader = new Vtu2TetrFileReader();
    reader->readFile(engine->getFileFolderLookupService().lookupFile(params[PARAM_FILE]),
                        mesh, dispatcher, mesh->getBody()->getEngine()->getRank(), true);
    delete reader;

    mesh->preProcess();
}

void gcm::Vtu2MeshZoneLoader::preLoadMesh(Params params, AABB* scene, int& sliceDirection, int& numberOfNodes) {
    if (params.find(PARAM_FILE) == params.end()) {
        THROW_INVALID_ARG("Vtu file name was not provided");
    }
    Vtu2TetrFileReader* reader = new Vtu2TetrFileReader();
    reader->preReadFile(engine->getFileFolderLookupService().lookupFile(params[PARAM_FILE]), scene, sliceDirection, numberOfNodes);
    delete reader;
}
