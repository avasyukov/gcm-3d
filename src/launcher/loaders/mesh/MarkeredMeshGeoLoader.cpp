#include "launcher/loaders/mesh/MarkeredMeshGeoLoader.hpp"
#include "libgcm/mesh/euler/markers/MarkeredSurfaceGeoGenerator.hpp"

#include "libgcm/Engine.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace gcm;

using boost::lexical_cast;
using boost::split;
using boost::is_any_of;
using std::string;
using std::vector;

const string launcher::MarkeredMeshGeoLoader::MESH_TYPE = "markered-mesh-geo";

void launcher::MarkeredMeshGeoLoader::parseDesc(const xml::Node& desc, string& id, vector3u& cellsNum, vector3r& cellSize, string& file)
{
    id = desc["id"];
    file = desc["file"];

    vector<string> strs;

    string _cellsNum = desc["cellsNum"];
    split(strs, _cellsNum, is_any_of(";"));

    assert_eq(strs.size(), 3);
    cellsNum.x = lexical_cast<int>(strs[0]);
    cellsNum.y = lexical_cast<int>(strs[1]);
    cellsNum.z = lexical_cast<int>(strs[2]);

    strs.clear();

    string _cellSize = desc["cellSize"];
    split(strs, _cellSize, is_any_of(";"));

    assert_eq(strs.size(), 3);
    cellSize.x = lexical_cast<real>(strs[0]);
    cellSize.y = lexical_cast<real>(strs[1]);
    cellSize.z = lexical_cast<real>(strs[2]);
}

void launcher::MarkeredMeshGeoLoader::preLoadMesh(const xml::Node& desc, AABB& aabb, int& sliceDirection, int& numberOfNodes)
{
    // Nothing to do here
}

void launcher::MarkeredMeshGeoLoader::loadMesh(const xml::Node& desc, MarkeredMesh* mesh)
{
    string id, file;
    vector3u cellsNum;
    vector3r cellSize;


    parseDesc(desc, id, cellsNum, cellSize, file);

    auto surface = MarkeredSurfaceGeoGenerator::getInstance().generate(file);

    mesh->setSurface(surface);
    mesh->setDimensions(cellsNum);
    mesh->setCellSize(cellSize);
    
    mesh->preProcess();
}
