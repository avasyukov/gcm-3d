#include "launcher/loaders/mesh/Vtu2MeshLoader.hpp"
#include "launcher/util/FileFolderLookupService.hpp"
#include "libgcm/Engine.hpp"

#include <boost/lexical_cast.hpp>

using boost::lexical_cast;

const string launcher::Vtu2MeshLoader::MESH_TYPE = "vtu2";

void launcher::Vtu2MeshLoader::parseDesc(const xml::Node& desc, string& id, string& fileName)
{
    auto& ffls = FileFolderLookupService::getInstance();

    id = desc["id"];
    fileName = ffls.lookupFile(desc["file"]);
}

void launcher::Vtu2MeshLoader::preLoadMesh(const xml::Node& desc, AABB& aabb, int& sliceDirection, int& numberOfNodes)
{
    string fileName, id;
    
    parseDesc(desc, id, fileName);

    gcm::Vtu2MeshLoader::getInstance().preLoadMesh(&aabb, sliceDirection, numberOfNodes, fileName);
}

void launcher::Vtu2MeshLoader::loadMesh(const xml::Node& desc, TetrMeshSecondOrder* mesh)
{
    string fileName, id;
    
    parseDesc(desc, id, fileName);

    gcm::Vtu2MeshLoader::getInstance().loadMesh(mesh, Engine::getInstance().getDispatcher(), fileName);
}
