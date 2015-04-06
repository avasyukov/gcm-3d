#include "launcher/loaders/mesh/Ani3D2MeshLoader.hpp"
#include "launcher/util/FileFolderLookupService.hpp"

#include <boost/lexical_cast.hpp>

using namespace gcm;

using boost::lexical_cast;
using std::string;

const string launcher::Ani3D2MeshLoader::MESH_TYPE = "ani3d2";

void launcher::Ani3D2MeshLoader::parseDesc(const xml::Node& desc, string& id, string& fileName)
{
    auto& ffls = FileFolderLookupService::getInstance();

    id = desc["id"];
    fileName = ffls.lookupFile(desc["file"]);
}

void launcher::Ani3D2MeshLoader::preLoadMesh(const xml::Node& desc, AABB& aabb, int& sliceDirection, int& numberOfNodes)
{
    string fileName, id;
    
    parseDesc(desc, id, fileName);

    gcm::Ani3D2MeshLoader::getInstance().preLoadMesh(&aabb, sliceDirection, numberOfNodes, fileName);
}

void launcher::Ani3D2MeshLoader::loadMesh(const xml::Node& desc, TetrMeshSecondOrder* mesh)
{
    string fileName, id;
    
    parseDesc(desc, id, fileName);

    gcm::Ani3D2MeshLoader::getInstance().loadMesh(mesh, Engine::getInstance().getDispatcher(), fileName);
}
