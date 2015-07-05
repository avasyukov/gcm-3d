#include "libgcm/mesh/tetr/AniVtuMeshLoader.hpp"

#include "libgcm/Engine.hpp"

using namespace gcm;
using std::string;

AniVtuMeshLoader::AniVtuMeshLoader() {
    INIT_LOGGER("gcm.AniVtuMeshLoader");
}

AniVtuMeshLoader::~AniVtuMeshLoader() {
}

void AniVtuMeshLoader::loadMesh(TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher, const string& fileName)
{
    AniVtuTetrFileReader* reader = new AniVtuTetrFileReader();
    reader->readFile(fileName, mesh, dispatcher, Engine::getInstance().getRank());
    delete reader;

    mesh->preProcess();
}

void AniVtuMeshLoader::preLoadMesh(AABB* scene, int& sliceDirection, int& numberOfNodes, const string& fileName) {
    AniVtuTetrFileReader* reader = new AniVtuTetrFileReader();
    reader->preReadFile(fileName, scene, sliceDirection, numberOfNodes);
    delete reader;
}
