#include "launcher/loaders/mesh/Msh2MeshLoader.hpp"
#include "launcher/util/FileFolderLookupService.hpp"

#include <boost/lexical_cast.hpp>

using boost::lexical_cast;

const string launcher::Msh2MeshLoader::MESH_TYPE = "msh2";

void launcher::Msh2MeshLoader::parseDesc(const xml::Node& desc, string& id, string& fileName)
{
    auto& ffls = FileFolderLookupService::getInstance();

    id = desc["id"];
    fileName = ffls.lookupFile(desc["file"]);
}

void launcher::Msh2MeshLoader::preLoadMesh(const xml::Node& desc, AABB& aabb, int& sliceDirection, int& numberOfNodes)
{
    string fileName, id;
    
    parseDesc(desc, id, fileName);

    gcm::Msh2MeshLoader::getInstance().preLoadMesh(&aabb, sliceDirection, numberOfNodes, fileName);
}

void launcher::Msh2MeshLoader::loadMesh(const xml::Node& desc, TetrMeshSecondOrder* mesh)
{
    string fileName, id;
    
    parseDesc(desc, id, fileName);

    gcm::Msh2MeshLoader::getInstance().loadMesh(mesh, Engine::getInstance().getDispatcher(), fileName);
}
