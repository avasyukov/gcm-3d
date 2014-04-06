#include "launcher/loaders/mesh/CubicMeshLoader.hpp"
#include "libgcm/mesh/cube/BasicCubicMeshGenerator.hpp"

#include "libgcm/Engine.hpp"

#include <boost/lexical_cast.hpp>

using boost::lexical_cast;

const string launcher::CubicMeshLoader::MESH_TYPE = "cube";

void launcher::CubicMeshLoader::parseDesc(const xml::Node& desc, string& id, float& h, int& num)
{
    id = desc["id"];
    h = lexical_cast<float>(desc["h"]);
    num = lexical_cast<int>(desc["num"]);
}

void launcher::CubicMeshLoader::preLoadMesh(const xml::Node& desc, AABB& aabb, int& sliceDirection, int& numberOfNodes)
{
    string id;
    float h;
    int num;

    parseDesc(desc, id, h, num);

    gcm::BasicCubicMeshGenerator::getInstance().preLoadMesh(&aabb, sliceDirection, numberOfNodes, h, num);
}

void launcher::CubicMeshLoader::loadMesh(const xml::Node& desc, BasicCubicMesh* mesh)
{
    string id;
    float h;
    int num;

    parseDesc(desc, id, h, num);

    gcm::BasicCubicMeshGenerator::getInstance().loadMesh(mesh, Engine::getInstance().getDispatcher(), h, num);
}
