#include "libgcm/mesh/tetr/Vtu2MeshZoneLoader.hpp"

#include "libgcm/Engine.hpp"

using namespace gcm;
using std::string;

Vtu2MeshZoneLoader::Vtu2MeshZoneLoader() {
    INIT_LOGGER("gcm.Vtu2MeshZoneLoader");
}

Vtu2MeshZoneLoader::~Vtu2MeshZoneLoader() {
}

void Vtu2MeshZoneLoader::loadMesh(TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher, const string& fileName)
{
    Vtu2TetrFileReader* reader = new Vtu2TetrFileReader();
    reader->readFile(fileName, mesh, dispatcher, Engine::getInstance().getRank(), true);
    delete reader;

    mesh->preProcess();
}

void Vtu2MeshZoneLoader::preLoadMesh(AABB* scene, int& sliceDirection, int& numberOfNodes, const string& fileName) {
    Vtu2TetrFileReader* reader = new Vtu2TetrFileReader();
    reader->preReadFile(fileName, scene, sliceDirection, numberOfNodes);
    delete reader;
}
