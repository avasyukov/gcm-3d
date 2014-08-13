#include "launcher/loaders/mesh/MarkeredMeshNGGeoLoader.hpp"
#include "libgcm/mesh/markers/MarkeredSurfaceGeoGenerator.hpp"

#include "libgcm/Engine.hpp"

#include <boost/lexical_cast.hpp>

using boost::lexical_cast;

const string launcher::MarkeredMeshNGGeoLoader::MESH_TYPE = "markered-mesh-ng-geo";

void launcher::MarkeredMeshNGGeoLoader::parseDesc(const xml::Node& desc, string& id, int& cellsNum, string& file)
{
    id = desc["id"];
    cellsNum = lexical_cast<int>(desc["cellsNum"]);
    file = desc["file"];
}

void launcher::MarkeredMeshNGGeoLoader::preLoadMesh(const xml::Node& desc, AABB& aabb, int& sliceDirection, int& numberOfNodes)
{
    // Nothing to do here
}

void launcher::MarkeredMeshNGGeoLoader::loadMesh(const xml::Node& desc, MarkeredMeshNG* mesh)
{
    string id, file;
    int cellsNum;

    parseDesc(desc, id, cellsNum, file);

    auto surface = gcm::MarkeredSurfaceGeoGenerator::getInstance().generate(file);
    
    mesh->setMeshElems(cellsNum);
    mesh->setSurface(surface);
    mesh->generateMesh();
}
