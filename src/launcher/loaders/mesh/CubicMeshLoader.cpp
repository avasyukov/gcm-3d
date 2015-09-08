#include "launcher/loaders/mesh/CubicMeshLoader.hpp"
#include "libgcm/mesh/cube/BasicCubicMeshGenerator.hpp"

#include "libgcm/Engine.hpp"

#include <boost/lexical_cast.hpp>

using namespace gcm;

using boost::lexical_cast;
using std::string;

const string launcher::CubicMeshLoader::MESH_TYPE = "cube";

void launcher::CubicMeshLoader::parseDesc(const xml::Node& desc, string& id, 
                                          float& h, int& numX, int& numY, int& numZ)
{
    id = desc["id"];
    h = lexical_cast<float>(desc["h"]);
    numX = lexical_cast<int>(desc["numX"]);
	numY = lexical_cast<int>(desc["numY"]);
	numZ = lexical_cast<int>(desc["numZ"]);
}

void launcher::CubicMeshLoader::preLoadMesh(const xml::Node& desc, AABB& aabb, int& sliceDirection, int& numberOfNodes)
{
    string id;
    float h;
    int numX, numY, numZ;

    parseDesc(desc, id, h, numX, numY, numZ);

    gcm::BasicCubicMeshGenerator::getInstance().preLoadMesh(&aabb, sliceDirection, numberOfNodes, h, numX, numY, numZ);
}

void launcher::CubicMeshLoader::loadMesh(const xml::Node& desc, BasicCubicMesh* mesh)
{
    string id;
    float h;
    int numX, numY, numZ;

    parseDesc(desc, id, h, numX, numY, numZ);

    gcm::BasicCubicMeshGenerator::getInstance().loadMesh(mesh, 
		Engine::getInstance().getDispatcher(), h, numX, numY, numZ);
}
