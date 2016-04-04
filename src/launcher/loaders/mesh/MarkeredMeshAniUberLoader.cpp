#include "launcher/loaders/mesh/MarkeredMeshAniUberLoader.hpp"
#include "libgcm/mesh/euler/markers/MarkeredSurfaceAniUberGenerator.hpp"

#include "libgcm/Engine.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace gcm;

using boost::lexical_cast;
using boost::split;
using boost::is_any_of;
using std::string;
using std::vector;
using std::pair;

const string launcher::MarkeredMeshAniUberLoader::MESH_TYPE = "markered-mesh-ani-uber";

void launcher::MarkeredMeshAniUberLoader::parseDesc(const xml::Node& desc, string& id, vector3u& cellsNum, vector3r& cellSize, string& source)
{
    id = desc["id"];
    source = desc["source"];

    vector<string> strs;

    strs.clear();

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

void launcher::MarkeredMeshAniUberLoader::preLoadMesh(const xml::Node& desc, AABB& aabb, int& sliceDirection, int& numberOfNodes)
{
    // Nothing to do here
}

void launcher::MarkeredMeshAniUberLoader::loadMesh(const xml::Node& desc, MarkeredMesh* mesh)
{
    string id;
    string source;
    vector3u cellsNum;
    vector3r cellSize;


    parseDesc(desc, id, cellsNum, cellSize, source);

    auto surface = MarkeredSurfaceAniUberGenerator::getInstance().generate(source);

    mesh->setSurface(surface);
    mesh->setDimensions(cellsNum);
    mesh->setCellSize(cellSize);
    
    mesh->preProcess();
}
