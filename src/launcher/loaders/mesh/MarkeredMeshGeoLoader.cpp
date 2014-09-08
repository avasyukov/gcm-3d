#include "launcher/loaders/mesh/MarkeredMeshGeoLoader.hpp"
#include "libgcm/mesh/markers/MarkeredSurfaceGeoGenerator.hpp"

#include "libgcm/Engine.hpp"

#include <boost/lexical_cast.hpp>

using boost::lexical_cast;

const string launcher::MarkeredMeshGeoLoader::MESH_TYPE = "markered-mesh-geo";

void launcher::MarkeredMeshGeoLoader::parseDesc(const xml::Node& desc, string& id, int& cellsNum, string& file)
{
    id = desc["id"];
    cellsNum = lexical_cast<int>(desc["cellsNum"]);
    file = desc["file"];
}

void launcher::MarkeredMeshGeoLoader::preLoadMesh(const xml::Node& desc, AABB& aabb, int& sliceDirection, int& numberOfNodes)
{
    // Nothing to do here
}

void launcher::MarkeredMeshGeoLoader::loadMesh(const xml::Node& desc, MarkeredMesh* mesh)
{
    string id, file;
    int cellsNum;

    parseDesc(desc, id, cellsNum, file);

    auto surface = gcm::MarkeredSurfaceGeoGenerator::getInstance().generate(file);
    
    mesh->setMeshElems(cellsNum);
    mesh->setSurface(surface);
    mesh->generateMesh();
}
