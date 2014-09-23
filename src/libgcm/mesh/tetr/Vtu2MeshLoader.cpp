#include "libgcm/mesh/tetr/Vtu2MeshLoader.hpp"

#include "libgcm/Engine.hpp"

using namespace gcm;
using std::string;

Vtu2MeshLoader::Vtu2MeshLoader() {
    INIT_LOGGER("gcm.Vtu2MeshLoader");
}

Vtu2MeshLoader::~Vtu2MeshLoader() {
}

void Vtu2MeshLoader::loadMesh(TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher, const string& fileName)
{
    Vtu2TetrFileReader* reader = new Vtu2TetrFileReader();
    reader->readFile(fileName, mesh, dispatcher, Engine::getInstance().getRank());
    delete reader;

    mesh->preProcess();
}

void Vtu2MeshLoader::preLoadMesh(AABB* scene, int& sliceDirection, int& numberOfNodes, const string& fileName) {
    Vtu2TetrFileReader* reader = new Vtu2TetrFileReader();
    reader->preReadFile(fileName, scene, sliceDirection, numberOfNodes);
    delete reader;
}
