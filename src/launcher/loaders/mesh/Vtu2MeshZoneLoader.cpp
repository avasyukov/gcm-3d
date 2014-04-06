#include "launcher/loaders/mesh/Vtu2MeshZoneLoader.hpp"
#include "launcher/util/FileFolderLookupService.hpp"
#include "libgcm/mesh/tetr/Vtu2MeshZoneLoader.hpp"
#include "libgcm/Engine.hpp"

#include <boost/lexical_cast.hpp>

using boost::lexical_cast;

const string launcher::Vtu2MeshZoneLoader::MESH_TYPE = "vtu2zone";

void launcher::Vtu2MeshZoneLoader::parseDesc(const xml::Node& desc, string& id, string& fileName)
{
    auto& ffls = FileFolderLookupService::getInstance();

    id = desc["id"];
    fileName = ffls.lookupFile(desc["file"]);
}

void launcher::Vtu2MeshZoneLoader::preLoadMesh(const xml::Node& desc, AABB& aabb, int& sliceDirection, int& numberOfNodes)
{
    string fileName, id;
    
    parseDesc(desc, id, fileName);

    gcm::Vtu2MeshZoneLoader::getInstance().preLoadMesh(&aabb, sliceDirection, numberOfNodes, fileName);
}

void launcher::Vtu2MeshZoneLoader::loadMesh(const xml::Node& desc, TetrMeshSecondOrder* mesh)
{
    string fileName, id;
    
    parseDesc(desc, id, fileName);

    gcm::Vtu2MeshZoneLoader::getInstance().loadMesh(mesh, Engine::getInstance().getDispatcher(), fileName);
}
