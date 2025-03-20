#include "launcher/loaders/mesh/RectangularMeshLoader.hpp"
#include "libgcm/mesh/cube/RectangularMeshGenerator.hpp"

#include "libgcm/Engine.hpp"

using namespace gcm;

using boost::lexical_cast;
using std::string;

const string launcher::RectangularMeshLoader::MESH_TYPE = "rectangle";

void launcher::RectangularMeshLoader::parseDesc(const xml::Node& desc, string& id, 
                                                  int& numX, int& numY, int& numZ, 
                                                  double& sx, double& sy, double& sz)
{
    id = desc["id"];
    numX = lexical_cast<int>(desc["numX"]);
	numY = lexical_cast<int>(desc["numY"]);
	numZ = lexical_cast<int>(desc["numZ"]);
    sz = lexical_cast<double>(desc["sz"]);
    sx = lexical_cast<double>(desc["sx"]);
    sy = lexical_cast<double>(desc["sy"]);
}


void launcher::RectangularMeshLoader::loadMesh(const xml::Node& desc, RectangularMesh* mesh)
{
    string id;
    int numX, numY, numZ;
    double sx, sy, sz;

    parseDesc(desc, id, numX, numY, numZ, sx, sy, sz);

    gcm::RectangularMeshGenerator::getInstance().loadMesh(mesh, 
		Engine::getInstance().getDispatcher(), numX, numY, numZ, sx, sy, sz);
}