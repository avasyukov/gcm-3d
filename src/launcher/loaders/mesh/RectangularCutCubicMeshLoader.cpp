#include "launcher/loaders/mesh/RectangularCutCubicMeshLoader.hpp"
#include "libgcm/mesh/cube/RectangularCutCubicMeshGenerator.hpp"
#include "launcher/util/xml.hpp"

#include "libgcm/Engine.hpp"

#include <boost/lexical_cast.hpp>

using namespace gcm;

using boost::lexical_cast;
using std::string;

const string launcher::RectangularCutCubicMeshLoader::MESH_TYPE = "cut_cube";

void launcher::RectangularCutCubicMeshLoader::parseDesc(const xml::Node& desc, string& id, 
                                          float& h, int& numX, int& numY, int& numZ)
{
    id = desc["id"];
    h = lexical_cast<float>(desc["h"]);
    numX = lexical_cast<int>(desc["numX"]);
	numY = lexical_cast<int>(desc["numY"]);
	numZ = lexical_cast<int>(desc["numZ"]);
}

void launcher::RectangularCutCubicMeshLoader::preLoadMesh(const xml::Node& desc,
	AABB& aabb, int& sliceDirection, int& numberOfNodes)
{
    string id;
    float h;
    int numX, numY, numZ;

    parseDesc(desc, id, h, numX, numY, numZ);

    gcm::RectangularCutCubicMeshGenerator::getInstance().preLoadMesh(&aabb, sliceDirection,
		numberOfNodes, h, numX, numY, numZ);
}

void launcher::RectangularCutCubicMeshLoader::loadMesh(const xml::Node& desc,
	RectangularCutCubicMesh* mesh)
{
    string id;
    float h;
    int numX, numY, numZ;

    parseDesc(desc, id, h, numX, numY, numZ);
	
	xml::Node cutNode = desc.getChildByName("cut");
	int minX = lexical_cast<int>(cutNode["minX"]);
	int minY = lexical_cast<int>(cutNode["minY"]);
	int minZ = lexical_cast<int>(cutNode["minZ"]);
	int maxX = lexical_cast<int>(cutNode["maxX"]);
	int maxY = lexical_cast<int>(cutNode["maxY"]);
	int maxZ = lexical_cast<int>(cutNode["maxZ"]);

    gcm::RectangularCutCubicMeshGenerator::getInstance().loadMesh(mesh, 
		Engine::getInstance().getDispatcher(), h, numX, numY, numZ,
		                                          minX, minY, minZ,
		                                          maxX, maxY, maxZ);
}
