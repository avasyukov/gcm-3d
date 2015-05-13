#include "launcher/loaders/mesh/MarkeredMeshAniLoader.hpp"
#include "libgcm/mesh/euler/markers/MarkeredSurfaceAniGenerator.hpp"

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

const string launcher::MarkeredMeshAniLoader::MESH_TYPE = "markered-mesh-ani";

void launcher::MarkeredMeshAniLoader::parseDesc(const xml::Node& desc, string& id, vector3u& cellsNum, vector3r& cellSize, vector<pair<string, string>>& source)
{
    id = desc["id"];
    string _source = desc["source"];

    vector<string> strs;

    split(strs, _source, is_any_of(" "));
    for (const auto& s: strs)
    {
        vector<string> _pair;
        split(_pair, s, is_any_of(":"));
        source.push_back(pair<string, string>(_pair[0], _pair[1]));
    }

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

void launcher::MarkeredMeshAniLoader::preLoadMesh(const xml::Node& desc, AABB& aabb, int& sliceDirection, int& numberOfNodes)
{
    // Nothing to do here
}

void launcher::MarkeredMeshAniLoader::loadMesh(const xml::Node& desc, MarkeredMesh* mesh)
{
    vector<pair<string, string>> source;
    string id;
    vector3u cellsNum;
    vector3r cellSize;


    parseDesc(desc, id, cellsNum, cellSize, source);

    auto surface = MarkeredSurfaceAniGenerator::getInstance().generate(source);

    mesh->setSurface(surface);
    mesh->setDimensions(cellsNum);
    mesh->setCellSize(cellSize);
    
    mesh->preProcess();
}
