#include "libgcm/mesh/markers/MarkeredSurfaceGeoGenerator.hpp"

#include "libgcm/Engine.hpp"
#include "libgcm/elem/TriangleFirstOrder.hpp"
#include "libgcm/util/Assertion.hpp"

#include <gmsh/Gmsh.h>
#include <gmsh/GModel.h>
#include <gmsh/GEntity.h>
#include <gmsh/MElement.h>

#include <vector>

using namespace gcm;
using namespace std;

MarkeredSurfaceGeoGenerator::MarkeredSurfaceGeoGenerator()
{
    INIT_LOGGER("gcm.mesh.markers.MarkeredSurfaceGeoGenerator");
}

MarkeredSurface MarkeredSurfaceGeoGenerator::generate(string fileName, gcm::real size)
{
    Engine& engine = Engine::getInstance();

    LOG_INFO("Generating markered surface from Geo file " << fileName);
    GmshSetOption("General", "Terminal", 1.0);
    GmshSetOption("General", "Verbosity", engine.getGmshVerbosity());
    GmshSetOption("General", "ExpertMode", 1.0);

    GModel gmshModel;
    gmshModel.setFactory("Gmsh");
    gmshModel.readGEO(fileName);

    float clmin, clmax;

    if (size > 0)
    {
        clmin = size*0.8;
        clmax = size*1.2;
    }
    else
    {
        auto min = gmshModel.bounds().min();
        auto max = gmshModel.bounds().max();
        auto d = min.distance(max);
        clmin = d/50;
        clmax = d/30;
    }

    GmshSetOption("Mesh", "CharacteristicLengthMin", clmin);
    GmshSetOption("Mesh", "CharacteristicLengthMax", clmax);

    gmshModel.mesh(2);

    vector<GEntity*> entities;
    gmshModel.getEntities(entities);

    auto nverts = gmshModel.getNumMeshVertices();

    vector<CalcNode> markers;
    vector<TriangleFirstOrder> faces;
    vector<int> regions;

    int *newVertNums = new int[nverts+1];

    for (int i = 0; i <= nverts; i++)
        newVertNums[i] = -1;


    MVertex* verts[3];

    int nf = 0;
    int nv = 0;

    for (auto e: entities)
    {
        if (e->geomType() == GEntity::RuledSurface)
        {
            regions.push_back(e->getNumMeshElements());
            for (unsigned int i = 0; i < e->getNumMeshElements(); i++)
            {
                auto elem = e->getMeshElement(i);
                assert_eq(elem->getNumFaces(), 1);
                
                auto face = elem->getFace(0);

                for (int j = 0; j < 3; j++)
                    verts[j] = face.getVertex(j);

                int v[3];
                for (int j = 0; j < 3; j++)
                {
                    if (newVertNums[verts[j]->getNum()] == -1)
                    {
                        newVertNums[verts[j]->getNum()] = nv;
                        markers.push_back(CalcNode(nv, verts[j]->x(), verts[j]->y(), verts[j]->z()));
                        nv++;
                    }
                    v[j] = newVertNums[verts[j]->getNum()];
                }
                faces.push_back(TriangleFirstOrder(nf++, v));
            }
        }
    }

    delete[] newVertNums;

    return MarkeredSurface(markers, faces, regions);
}
