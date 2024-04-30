#include "libgcm/mesh/euler/markers/MarkeredSurfaceGeoGenerator.hpp"

#include "libgcm/Engine.hpp"
#include "libgcm/elem/TriangleFirstOrder.hpp"
#include "libgcm/util/Assertion.hpp"

#include <gmsh.h>

#include <vector>
#include <algorithm>

using namespace gcm;
using std::vector;
using std::string;
using std::min;

MarkeredSurfaceGeoGenerator::MarkeredSurfaceGeoGenerator()
{
    INIT_LOGGER("gcm.mesh.markers.MarkeredSurfaceGeoGenerator");
}

MarkeredSurface MarkeredSurfaceGeoGenerator::generate(string fileName, real size)
{
    const unsigned int GMSH_TRIANGLE_CODE = 2;

    Engine& engine = Engine::getInstance();

    LOG_INFO("Generating markered surface from Geo file " << fileName);

    gmsh::initialize();
    gmsh::open(fileName);
    gmsh::model::geo::synchronize();

    double clmin, clmax;

    if (size > 0)
    {
        clmin = size * 0.8;
        clmax = size * 1.2;
    }
    else
    {
        double xmin;
        double ymin;
        double zmin;
        double xmax;
        double ymax;
        double zmax;
        gmsh::model::getBoundingBox(-1, -1, xmin, ymin, zmin, xmax, ymax, zmax);

        auto d1 = xmax - xmin;
        auto d2 = ymax - ymin;
        auto d3 = zmax - zmin;
        auto d = min({d1, d2, d3});

        clmin = d / 10;
        clmax = d / 5;
    }

    // Set desired element size
    gmsh::option::setNumber("Mesh.CharacteristicLengthMin", clmin);
    gmsh::option::setNumber("Mesh.CharacteristicLengthMax", clmax);

    // Build surface mesh
    gmsh::model::mesh::generate(2);

    // We need to fill these two
    vector<CalcNode> markers;
    vector<TriangleFirstOrder> faces;
//    vector<int> regions;

    // Get nodes data from gmsh
    std::vector<double> nodesCoord;
    std::vector<std::size_t> nodeTags;
    std::vector<double> parametricCoord;
    gmsh::model::mesh::getNodes(nodeTags, nodesCoord, parametricCoord);

    // Get triangles data from gmsh
    std::vector<std::size_t>* trianglesNodesTags = nullptr;
    std::vector<int> elementTypes;
    std::vector<std::vector<std::size_t>> elementTags;
    std::vector<std::vector<std::size_t>> elementNodeTags;
    gmsh::model::mesh::getElements(elementTypes, elementTags, elementNodeTags);
    for(unsigned int i = 0; i < elementTypes.size(); i++) {
        if(elementTypes[i] != GMSH_TRIANGLE_CODE)
            continue;
        trianglesNodesTags = &elementNodeTags[i];
    }

    if(trianglesNodesTags == nullptr) {
        gmsh::finalize();
        THROW_UNSUPPORTED("Can not find triangles data. Exiting.");
    }

    // На всякий случай проверим, что номера узлов идут подряд и без пробелов
    for(int i = 0; i < nodeTags.size(); ++i) {
        // Индексация в gmsh начинается с 1, а не с нуля. Ну штош, значит так.
        assert(i == nodeTags[i] - 1);
    }
    // И ещё проверим, что в треугольниках что-то похожее на правду лежит.
    assert(trianglesNodesTags->size() % 3 == 0);

    LOG_INFO("The model has " << nodeTags.size() << " nodes and " << trianglesNodesTags->size() / 3 << " triangles");

    // Напоминалка: индексация в gmsh начинается с 1
    int *newVertNums = new int[nodeTags.size() + 1];
    for (int i = 0; i < nodeTags.size() + 1; i++)
        newVertNums[i] = -1;

    int nf = 0;
    int nv = 0;

    // Пройдём по элементам в модели gmsh
    for(unsigned int i = 0; i < trianglesNodesTags->size() / 3; i++) {
        int v[3];
        for (int j = 0; j < 3; j++)
        {
            std::size_t vertex_tag = trianglesNodesTags->at(i * 3 + j);
            if (newVertNums[vertex_tag] == -1)
            {
                newVertNums[vertex_tag] = nv;
                // Напоминалка: индексация в gmsh начинается с 1
                double pointX = nodesCoord[vertex_tag - 1];
                double pointY = nodesCoord[vertex_tag - 1 + 1];
                double pointZ = nodesCoord[vertex_tag - 1 + 2];
                markers.push_back(CalcNode(nv, vector3r(pointX, pointY, pointZ)));
                nv++;
            }
            v[j] = newVertNums[vertex_tag];
        }
        faces.push_back(TriangleFirstOrder(nf++, v));
    }

//    vector<GEntity*> entities;
//    gmshModel.getEntities(entities);
//
//    auto nverts = gmshModel.getNumMeshVertices();
//
//    vector<CalcNode> markers;
//    vector<TriangleFirstOrder> faces;
//    vector<int> regions;
//
//    int *newVertNums = new int[nverts+1];
//
//    for (int i = 0; i <= nverts; i++)
//        newVertNums[i] = -1;
//
//
//    MVertex* verts[3];
//
//    int nf = 0;
//    int nv = 0;
//
//    for (auto e: entities)
//    {
//        if (e->geomType() == GEntity::RuledSurface || e->geomType() == GEntity::Plane)
//        {
//            regions.push_back(e->getNumMeshElements());
//            for (unsigned int i = 0; i < e->getNumMeshElements(); i++)
//            {
//                auto elem = e->getMeshElement(i);
//                assert_eq(elem->getNumFaces(), 1);
//
//                auto face = elem->getFace(0);
//
//                for (int j = 0; j < 3; j++)
//                    verts[j] = face.getVertex(j);
//
//                int v[3];
//                for (int j = 0; j < 3; j++)
//                {
//                    if (newVertNums[verts[j]->getNum()] == -1)
//                    {
//                        newVertNums[verts[j]->getNum()] = nv;
//                        markers.push_back(CalcNode(nv, vector3r(verts[j]->x(), verts[j]->y(), verts[j]->z())));
//                        nv++;
//                    }
//                    v[j] = newVertNums[verts[j]->getNum()];
//                }
//                faces.push_back(TriangleFirstOrder(nf++, v));
//            }
//        }
//    }
//
//    assert_gt(markers.size(), 0);
//    assert_gt(faces.size(), 0);
//

    delete[] newVertNums;

    gmsh::finalize();

    return MarkeredSurface(markers, faces/*, regions*/);
}
