#include "libgcm/mesh/euler/markers/MarkeredSurfaceAniUberGenerator.hpp"

#include "libgcm/Engine.hpp"
#include "libgcm/elem/TriangleFirstOrder.hpp"
#include "libgcm/util/Assertion.hpp"
#include "libgcm/mesh/tetr/TetrMeshFirstOrder.hpp"

#include "libgcm/util/formats/Ani3DTetrFileReader.hpp"

#include <vector>
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

using namespace gcm;
using std::vector;
using std::pair;
using std::string;
using std::min;
using std::istringstream;
using std::unordered_map;
using std::unordered_set;

MarkeredSurfaceAniUberGenerator::MarkeredSurfaceAniUberGenerator()
{
    INIT_LOGGER("gcm.mesh.markers.MarkeredSurfaceAniUberGenerator");
}

MarkeredSurface MarkeredSurfaceAniUberGenerator::generate(string& source)
{
    Engine& engine = Engine::getInstance();

    vector<CalcNode> markers;
    vector<TriangleFirstOrder> faces;
    vector<int> regions;
    vector<pair<string, TetrMeshFirstOrder*>> meshes;

    Ani3DTetrFileReader reader;

    int n = 0;

    TetrMeshFirstOrder* mesh = new TetrMeshFirstOrder();
    meshes.push_back(pair<string, TetrMeshFirstOrder*>("", mesh));
    reader.readFile(source, mesh, engine.getDispatcher(), 0, true);
    mesh->setRheology(0);
    mesh->preProcessGeometry();
    unordered_set<int> nodes;

    int idx = faces.size();
    for (auto& tri: mesh->border1)
    {
        faces.push_back(tri);
        nodes.insert(tri.verts[0]);
        nodes.insert(tri.verts[1]);
        nodes.insert(tri.verts[2]);
    }
    regions.push_back(mesh->border1.size());
    LOG_INFO(mesh->border1.size());
    unordered_map<int, int> nummap;
    for (int x: nodes)
    { 
        const auto& node = mesh->getNode(x);
        markers.push_back(CalcNode(n, node.coords));
        nummap[x] = n;
        n++;
    }
    for (uint i = idx; i < faces.size(); i++)
    {
        auto& tri = faces[i];
        tri.verts[0] = nummap[tri.verts[0]];
        tri.verts[1] = nummap[tri.verts[1]];
        tri.verts[2] = nummap[tri.verts[2]];
    }

    LOG_INFO("Done");


    return MarkeredSurface(markers, faces, regions, meshes);
}
