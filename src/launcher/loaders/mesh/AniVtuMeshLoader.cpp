#include "launcher/loaders/mesh/AniVtuMeshLoader.hpp"
#include "launcher/util/FileFolderLookupService.hpp"
#include "libgcm/Engine.hpp"

#include <boost/lexical_cast.hpp>

using namespace gcm;
using boost::lexical_cast;
using std::string;

const string launcher::AniVtuMeshLoader::MESH_TYPE = "anivtu";

void launcher::AniVtuMeshLoader::parseDesc(const xml::Node& desc, string& id, string& fileName)
{
    auto& ffls = FileFolderLookupService::getInstance();

    id = desc["id"];
    fileName = ffls.lookupFile(desc["file"]);
}

void launcher::AniVtuMeshLoader::preLoadMesh(const xml::Node& desc, AABB& aabb, int& sliceDirection, int& numberOfNodes)
{
    string fileName, id;
    
    parseDesc(desc, id, fileName);

    gcm::AniVtuMeshLoader::getInstance().preLoadMesh(&aabb, sliceDirection, numberOfNodes, fileName);
}

void launcher::AniVtuMeshLoader::loadMesh(const xml::Node& desc, TetrMeshSecondOrder* mesh)
{
    string fileName, id;
    
    parseDesc(desc, id, fileName);

    gcm::AniVtuMeshLoader::getInstance().loadMesh(mesh, Engine::getInstance().getDispatcher(), fileName);
}
