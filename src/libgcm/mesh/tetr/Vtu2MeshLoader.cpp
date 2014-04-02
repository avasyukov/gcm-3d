#include "libgcm/mesh/tetr/Vtu2MeshLoader.hpp"

gcm::Vtu2MeshLoader::Vtu2MeshLoader() {
    INIT_LOGGER("gcm.Vtu2MeshLoader");
}

gcm::Vtu2MeshLoader::~Vtu2MeshLoader() {
}

void gcm::Vtu2MeshLoader::loadMesh(TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher, const string& fileName)
{
    Vtu2TetrFileReader* reader = new Vtu2TetrFileReader();
    reader->readFile(fileName, mesh, dispatcher, mesh->getBody()->getEngine()->getRank());
    delete reader;

    mesh->preProcess();
}

void gcm::Vtu2MeshLoader::preLoadMesh(AABB* scene, int& sliceDirection, int& numberOfNodes, const string& fileName) {
    Vtu2TetrFileReader* reader = new Vtu2TetrFileReader();
    reader->preReadFile(fileName, scene, sliceDirection, numberOfNodes);
    delete reader;
}
