#include "launcher/loaders/mesh/StlMeshLoader.hpp"
#include "launcher/util/FileFolderLookupService.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

using namespace gcm;
using boost::lexical_cast;
using std::string;

const string launcher::StlMeshLoader::MESH_TYPE = "stl";

void launcher::StlMeshLoader::parseDesc(const xml::Node& desc, string& id, string& fileName, float& tetrSize)
{
    auto& ffls = FileFolderLookupService::getInstance();

    id = desc["id"];
    fileName = ffls.lookupFile(desc["file"]);
    tetrSize = lexical_cast<float>(desc["tetrSize"]);
}

void launcher::StlMeshLoader::preLoadMesh(const xml::Node& desc, AABB& aabb, int& sliceDirection, int& numberOfNodes)
{
    string fileName, id;
    float tetrSize;

    parseDesc(desc, id, fileName, tetrSize);
    string localName = boost::filesystem::basename(fileName) + boost::filesystem::extension(fileName);
    //std::cout << "Working with " << localName  << " from " << fileName << std::endl;

    ofstream geofile;
    geofile.open(fileName + ".geo");
    geofile << "Merge \"" << localName << "\";\n"
            << "Surface Loop(2) = {1};\n"
            << "Volume(3) = {2};";
    geofile.close();

    gcm::Geo2MeshLoader::getInstance().preLoadMesh(&aabb, sliceDirection, numberOfNodes, fileName + ".geo", tetrSize);
}

void launcher::StlMeshLoader::loadMesh(const xml::Node& desc, TetrMeshSecondOrder* mesh)
{
    string fileName, id;
    float tetrSize;

    parseDesc(desc, id, fileName, tetrSize);

    gcm::Geo2MeshLoader::getInstance().loadMesh(mesh, Engine::getInstance().getDispatcher(), fileName + ".geo", tetrSize);
}
