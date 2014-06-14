#include "launcher/loaders/mesh/MarkeredBoxMeshLoader.hpp"
#include "libgcm/mesh/markers/MarkeredBoxMeshGenerator.hpp"

#include "libgcm/Engine.hpp"

#include <boost/lexical_cast.hpp>

using boost::lexical_cast;

const string launcher::MarkeredBoxMeshLoader::MESH_TYPE = "markered-box";

void launcher::MarkeredBoxMeshLoader::parseDesc(const xml::Node& desc, string& id, float& cubeEdge, float& meshEdge, int& cellsNum)
{
    id = desc["id"];
    cubeEdge = lexical_cast<float>(desc["cubeEdge"]);
    meshEdge = lexical_cast<float>(desc["meshEdge"]);
    cellsNum = lexical_cast<int>(desc["cellsNum"]);
}

void launcher::MarkeredBoxMeshLoader::preLoadMesh(const xml::Node& desc, AABB& aabb, int& sliceDirection, int& numberOfNodes)
{
    string id;
    float cubeEdge, meshEdge;
    int cellsNum;

    parseDesc(desc, id, cubeEdge, meshEdge, cellsNum);

    gcm::MarkeredBoxMeshGenerator::getInstance().preLoadMesh(&aabb, sliceDirection, numberOfNodes, cubeEdge, meshEdge, cellsNum);
}

void launcher::MarkeredBoxMeshLoader::loadMesh(const xml::Node& desc, MarkeredMesh* mesh)
{
    string id;
    float cubeEdge, meshEdge;
    int cellsNum;

    parseDesc(desc, id, cubeEdge, meshEdge, cellsNum);

    gcm::MarkeredBoxMeshGenerator::getInstance().loadMesh(mesh, Engine::getInstance().getDispatcher(), cubeEdge, meshEdge, cellsNum);
}
