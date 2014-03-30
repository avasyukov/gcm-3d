#include "launcher/loaders/mesh/Geo2MeshLoader.h"

#include <boost/lexical_cast.hpp>

using boost::lexical_cast;

const string launcher::Geo2MeshLoader::MESH_TYPE = "geo2";

void launcher::Geo2MeshLoader::parseDesc(const xml::Node& desc, string& id, string& fileName, float& tetrSize)
{
    auto& ffls = Engine::getInstance().getFileFolderLookupService();

    id = desc["id"];
    fileName = ffls.lookupFile(desc["file"]);
    tetrSize = lexical_cast<float>(desc["tetrSize"]);
}

void launcher::Geo2MeshLoader::preLoadMesh(const xml::Node& desc, AABB& aabb, int& sliceDirection, int& numberOfNodes)
{
    string fileName, id;
    float tetrSize;

    parseDesc(desc, id, fileName, tetrSize);

    gcm::Geo2MeshLoader::getInstance().preLoadMesh(&aabb, sliceDirection, numberOfNodes, fileName, tetrSize);
}

void launcher::Geo2MeshLoader::loadMesh(const xml::Node& desc, TetrMeshSecondOrder* mesh)
{
    string fileName, id;
    float tetrSize;

    parseDesc(desc, id, fileName, tetrSize);

    gcm::Geo2MeshLoader::getInstance().loadMesh(mesh, Engine::getInstance().getDispatcher(), fileName, tetrSize);
}
