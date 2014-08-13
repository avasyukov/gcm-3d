#include <cmath>
#include <gtest/gtest.h>

//#include "libgcm/mesh/markers/MarkeredMesh.hpp"
#include "libgcm/mesh/markers/MarkeredMeshNG.hpp"
//#include "libgcm/mesh/markers/MarkeredBoxMeshGenerator.hpp"
#include "libgcm/mesh/markers/MarkeredSurfaceGeoGenerator.hpp"
//#include "libgcm/rheology/Material.hpp"
#include "libgcm/Math.hpp"

#include "libgcm/snapshot/VTKMarkeredMeshNGSnapshotWriter.hpp"
/*
using namespace std;

class MarkeredBoxMesh : public ::testing::Test
{
protected:

    static void SetUpTestCase()
    {
        Engine::getInstance().addMaterial(makeMaterialPtr("default", 0, 0, 0, 0));
    }

};

TEST_F(MarkeredBoxMesh, getCellAABB)
{

    auto& mg = gcm::MarkeredBoxMeshGenerator::getInstance();

    MarkeredMesh m;
    mg.loadMesh(&m, nullptr, 2.0, 1.0, 2);

    AABB aabb;
    m.getCellAABB(0, aabb);

    ASSERT_FLOAT_EQ(-1.0, aabb.minX);
    ASSERT_FLOAT_EQ(-1.0, aabb.minY);
    ASSERT_FLOAT_EQ(-1.0, aabb.minZ);

    ASSERT_FLOAT_EQ(0.0, aabb.maxX);
    ASSERT_FLOAT_EQ(0.0, aabb.maxY);
    ASSERT_FLOAT_EQ(0.0, aabb.maxZ);

    m.getCellAABB(1, aabb);

    ASSERT_FLOAT_EQ(0.0, aabb.minX);
    ASSERT_FLOAT_EQ(-1.0, aabb.minY);
    ASSERT_FLOAT_EQ(-1.0, aabb.minZ);

    ASSERT_FLOAT_EQ(1.0, aabb.maxX);
    ASSERT_FLOAT_EQ(0.0, aabb.maxY);
    ASSERT_FLOAT_EQ(0.0, aabb.maxZ);

    m.getCellAABB(7, aabb);

    ASSERT_FLOAT_EQ(0.0, aabb.minX);
    ASSERT_FLOAT_EQ(0.0, aabb.minY);
    ASSERT_FLOAT_EQ(0.0, aabb.minZ);

    ASSERT_FLOAT_EQ(1.0, aabb.maxX);
    ASSERT_FLOAT_EQ(1.0, aabb.maxY);
    ASSERT_FLOAT_EQ(1.0, aabb.maxZ);

}

TEST_F(MarkeredBoxMesh, innerCells)
{

    auto& mg = gcm::MarkeredBoxMeshGenerator::getInstance();

    MarkeredMesh m;
    mg.loadMesh(&m, nullptr, 4.0, 2.0, 8);

    for (unsigned int i = 0; i < 8; i++)
        for (unsigned int j = 0; j < 8; j++)
            for (unsigned int k = 0; k < 8; k++) {
                unsigned int n = m.getCellNumber(i, j, k);
                bool in = i >= 2 && i <= 5 && j >= 2 && j <= 5 && k >= 2 && k <= 5;
                ASSERT_EQ(in, m.getCellInnerFlag(n));
            }

}

TEST_F(MarkeredBoxMesh, outlines)
{
    auto& mg = gcm::MarkeredBoxMeshGenerator::getInstance();

    MarkeredMesh m;
    mg.loadMesh(&m, nullptr, 2.0, 1.0, 20);

    AABB outline = m.getOutline();
    ASSERT_FLOAT_EQ(-0.5, outline.minX);
    ASSERT_FLOAT_EQ(-0.5, outline.minY);
    ASSERT_FLOAT_EQ(-0.5, outline.minZ);
    ASSERT_FLOAT_EQ(0.5, outline.maxX);
    ASSERT_FLOAT_EQ(0.5, outline.maxY);
    ASSERT_FLOAT_EQ(0.5, outline.maxZ);

    const AABB& meshOutline = m.getMeshOutline();
    ASSERT_FLOAT_EQ(-1.0, meshOutline.minX);
    ASSERT_FLOAT_EQ(-1.0, meshOutline.minY);
    ASSERT_FLOAT_EQ(-1.0, meshOutline.minZ);
    ASSERT_FLOAT_EQ(1.0, meshOutline.maxX);
    ASSERT_FLOAT_EQ(1.0, meshOutline.maxY);
    ASSERT_FLOAT_EQ(1.0, meshOutline.maxZ);

}

TEST_F(MarkeredBoxMesh, cellNighbours)
{
    auto& mg = gcm::MarkeredBoxMeshGenerator::getInstance();

    MarkeredMesh m;
    mg.loadMesh(&m, nullptr, 2.0, 1.0, 3);

    NeighbourCells cells;

    memset(cells, 0, sizeof (cells));
    ASSERT_EQ(7, m.getCellNeighbours(0, cells));
    ASSERT_EQ(1, cells[0]);
    ASSERT_EQ(3, cells[1]);
    ASSERT_EQ(4, cells[2]);
    ASSERT_EQ(9, cells[3]);
    ASSERT_EQ(10, cells[4]);
    ASSERT_EQ(12, cells[5]);
    ASSERT_EQ(13, cells[6]);

    memset(cells, 0, sizeof (cells));
    ASSERT_EQ(7, m.getCellNeighbours(26, cells));
    ASSERT_EQ(13, cells[0]);
    ASSERT_EQ(14, cells[1]);
    ASSERT_EQ(16, cells[2]);
    ASSERT_EQ(17, cells[3]);
    ASSERT_EQ(22, cells[4]);
    ASSERT_EQ(23, cells[5]);
    ASSERT_EQ(25, cells[6]);

    memset(cells, 0, sizeof (cells));
    ASSERT_EQ(26, m.getCellNeighbours(13, cells));
    for (int i = 0; i < 13; i++)
        ASSERT_EQ(i, cells[i]);
    for (int i = 14; i < 27; i++)
        ASSERT_EQ(i, cells[i - 1]);

}

TEST_F(MarkeredBoxMesh, markersGeneration)
{

    auto& mg = gcm::MarkeredBoxMeshGenerator::getInstance();

    MarkeredMesh m;
    mg.loadMesh(&m, nullptr, 2.0, 1.0, 20);

    AABB inner(m.getOutline());
    AABB outer(m.getOutline());
    for (int i = 0; i < 3; i++) {
        outer.min_coords[i] -= 0.1;
        outer.max_coords[i] += 0.1;
    }

    for (auto &marker : m.getMarkers()) {
        ASSERT_TRUE(outer.isInAABB(marker.x, marker.y, marker.z));
        ASSERT_FALSE(inner.isInAABB(marker.x, marker.y, marker.z));
    }
}

TEST_F(MarkeredBoxMesh, innerFlagsReconstruction)
{

    auto& mg = gcm::MarkeredBoxMeshGenerator::getInstance();

    MarkeredMesh m1, m2;
    mg.loadMesh(&m1, nullptr, 2.0, 1.0, 20);
    mg.loadMesh(&m2, nullptr, 2.0, 1.0, 20);

    m2.reconstructInnerFlags();

    for (unsigned int i = 0; i < 20 * 20 * 20; i++)
        ASSERT_EQ(m1.getCellInnerFlag(i), m2.getCellInnerFlag(i));

}

TEST_F(MarkeredBoxMesh, reconstructBorder)
{

    auto& mg = gcm::MarkeredBoxMeshGenerator::getInstance();

    MarkeredMesh m;
    mg.loadMesh(&m, nullptr, 2.0, 1.0, 20);

    m.reconstructBorder();

    for (unsigned int k = 0; k < 21; k++)
        for (unsigned int j = 0; j < 21; j++)
            for (unsigned int i = 0; i < 21; i++) {
                if ((i == 5 || i == 15) && j >= 5 && j <= 15 && k >= 5 && k <= 15)
                    ASSERT_TRUE(m.getNodeByLocalIndex(m.getPointNumber(i, j, k)).isBorder());
                else if ((j == 5 || j == 15) && i >= 5 && i <= 15 && k >= 5 && k <= 15)
                    ASSERT_TRUE(m.getNodeByLocalIndex(m.getPointNumber(i, j, k)).isBorder());
                else if ((k == 5 || k == 15) && i >= 5 && i <= 15 && j >= 5 && j <= 15)
                    ASSERT_TRUE(m.getNodeByLocalIndex(m.getPointNumber(i, j, k)).isBorder());
                else
                    ASSERT_FALSE(m.getNodeByLocalIndex(m.getPointNumber(i, j, k)).isBorder());
            }
}

TEST_F(MarkeredBoxMesh, markUnusedNodes)
{

    auto& mg = gcm::MarkeredBoxMeshGenerator::getInstance();

    MarkeredMesh m;
    mg.loadMesh(&m, nullptr, 2.0, 1.0, 20);

    m.reconstructBorder();
    m.markUnusedNodes();

    for (unsigned int k = 0; k < 21; k++)
        for (unsigned int j = 0; j < 21; j++)
            for (unsigned int i = 0; i < 21; i++)
                if (i >= 5 && i <= 15 && j >= 5 && j <= 15 && k >= 5 && k <= 15)
                    ASSERT_TRUE(m.getNodeByLocalIndex(m.getPointNumber(i, j, k)).isUsed());
                else
                    ASSERT_FALSE(m.getNodeByLocalIndex(m.getPointNumber(i, j, k)).isUsed());
}

int cmp(const void *a, const void *b)
{
    return *(static_cast<const unsigned int*> (a)) - *(static_cast<const unsigned int*> (b));
}

TEST_F(MarkeredBoxMesh, getCellsCommonPoints)
{

    auto& mg = gcm::MarkeredBoxMeshGenerator::getInstance();

    MarkeredMesh m;
    mg.loadMesh(&m, nullptr, 2.0, 1.0, 2);

    CommonPoints pts;

    ASSERT_EQ(4, m.getCellsCommonPoints(0, 1, pts));
    qsort(pts, 4, sizeof (int), cmp);
    ASSERT_EQ(1, pts[0]);
    ASSERT_EQ(4, pts[1]);
    ASSERT_EQ(10, pts[2]);
    ASSERT_EQ(13, pts[3]);

    ASSERT_EQ(4, m.getCellsCommonPoints(0, 4, pts));
    qsort(pts, 4, sizeof (int), cmp);
    ASSERT_EQ(9, pts[0]);
    ASSERT_EQ(10, pts[1]);
    ASSERT_EQ(12, pts[2]);
    ASSERT_EQ(13, pts[3]);

    ASSERT_EQ(2, m.getCellsCommonPoints(0, 3, pts));
    qsort(pts, 2, sizeof (int), cmp);
    ASSERT_EQ(4, pts[0]);
    ASSERT_EQ(13, pts[1]);

    ASSERT_EQ(1, m.getCellsCommonPoints(0, 7, pts));
    ASSERT_EQ(13, pts[0]);

}

TEST_F(MarkeredBoxMesh, findBorderNodeNormal)
{

    auto& mg = gcm::MarkeredBoxMeshGenerator::getInstance();

    MarkeredMesh m;
    mg.loadMesh(&m, nullptr, 3.0, 1.0, 3);

    float c = vectorNorm(1, 1, 1);
    float x, y, z;

    m.findBorderNodeNormal(21, &x, &y, &z, false);
    ASSERT_FLOAT_EQ(-1 / c, x);
    ASSERT_FLOAT_EQ(-1 / c, y);
    ASSERT_FLOAT_EQ(-1 / c, z);

    m.findBorderNodeNormal(22, &x, &y, &z, false);
    ASSERT_FLOAT_EQ(1 / c, x);
    ASSERT_FLOAT_EQ(-1 / c, y);
    ASSERT_FLOAT_EQ(-1 / c, z);

    m.findBorderNodeNormal(25, &x, &y, &z, false);
    ASSERT_FLOAT_EQ(-1 / c, x);
    ASSERT_FLOAT_EQ(1 / c, y);
    ASSERT_FLOAT_EQ(-1 / c, z);

    m.findBorderNodeNormal(26, &x, &y, &z, false);
    ASSERT_FLOAT_EQ(1 / c, x);
    ASSERT_FLOAT_EQ(1 / c, y);
    ASSERT_FLOAT_EQ(-1 / c, z);

    m.findBorderNodeNormal(37, &x, &y, &z, false);
    ASSERT_FLOAT_EQ(-1 / c, x);
    ASSERT_FLOAT_EQ(-1 / c, y);
    ASSERT_FLOAT_EQ(1 / c, z);

    m.findBorderNodeNormal(38, &x, &y, &z, false);
    ASSERT_FLOAT_EQ(1 / c, x);
    ASSERT_FLOAT_EQ(-1 / c, y);
    ASSERT_FLOAT_EQ(1 / c, z);

    m.findBorderNodeNormal(41, &x, &y, &z, false);
    ASSERT_FLOAT_EQ(-1 / c, x);
    ASSERT_FLOAT_EQ(1 / c, y);
    ASSERT_FLOAT_EQ(1 / c, z);

    m.findBorderNodeNormal(42, &x, &y, &z, false);
    ASSERT_FLOAT_EQ(1 / c, x);
    ASSERT_FLOAT_EQ(1 / c, y);
    ASSERT_FLOAT_EQ(1 / c, z);
}

TEST_F(MarkeredBoxMesh, interpolation)
{
    auto& mg = gcm::MarkeredBoxMeshGenerator::getInstance();

    MarkeredMesh m;
    mg.loadMesh(&m, nullptr, 4.0, 2.0, 4);

    m.reconstructBorder();
    m.markUnusedNodes();

    for (int i = 0; i < m.getNodesNumber(); i++) {
        auto& node = m.getNodeByLocalIndex(i);
        node.sxx = node.x;
        node.syy = node.y;
        node.szz = node.z;
    }

    CalcNode t;
    t.x = -1.0;
    t.y = -1.0;
    t.z = -1.0;

    CalcNode& o1 = m.getNodeByLocalIndex(31);

    bool isInnerPoint;

    m.interpolateNode(o1, -0.1, 0.0, 0.0, false, t, isInnerPoint);
    ASSERT_FALSE(isInnerPoint);
    ASSERT_FLOAT_EQ(o1.x, t.x);
    ASSERT_FLOAT_EQ(o1.y, t.y);
    ASSERT_FLOAT_EQ(o1.z, t.z);

    m.interpolateNode(o1, 0.0, -0.1, 0.0, false, t, isInnerPoint);
    ASSERT_FALSE(isInnerPoint);
    ASSERT_FLOAT_EQ(o1.x, t.x);
    ASSERT_FLOAT_EQ(o1.y, t.y);
    ASSERT_FLOAT_EQ(o1.z, t.z);

    m.interpolateNode(o1, 0.0, 0.0, -0.1, false, t, isInnerPoint);
    ASSERT_FALSE(isInnerPoint);
    ASSERT_FLOAT_EQ(o1.x, t.x);
    ASSERT_FLOAT_EQ(o1.y, t.y);
    ASSERT_FLOAT_EQ(o1.z, t.z);

    m.interpolateNode(o1, 0.1, 0.0, 0.0, false, t, isInnerPoint);
    ASSERT_TRUE(isInnerPoint);
    ASSERT_FLOAT_EQ(-0.9, t.x);
    ASSERT_FLOAT_EQ(-1.0, t.y);
    ASSERT_FLOAT_EQ(-1.0, t.z);
    ASSERT_NEAR(-0.9, t.sxx, 1e-6);
    ASSERT_NEAR(-1.0, t.syy, 1e-6);
    ASSERT_NEAR(-1.0, t.szz, 1e-6);

    m.interpolateNode(o1, 0.0, 0.1, 0.0, false, t, isInnerPoint);
    ASSERT_TRUE(isInnerPoint);
    ASSERT_FLOAT_EQ(-1.0, t.x);
    ASSERT_FLOAT_EQ(-0.9, t.y);
    ASSERT_FLOAT_EQ(-1.0, t.z);
    ASSERT_NEAR(-1.0, t.sxx, 1e-6);
    ASSERT_NEAR(-0.9, t.syy, 1e-6);
    ASSERT_NEAR(-1.0, t.szz, 1e-6);

    m.interpolateNode(o1, 0.0, 0.0, 0.1, false, t, isInnerPoint);
    ASSERT_TRUE(isInnerPoint);
    ASSERT_FLOAT_EQ(-1.0, t.x);
    ASSERT_FLOAT_EQ(-1.0, t.y);
    ASSERT_FLOAT_EQ(-0.9, t.z);
    ASSERT_NEAR(-1.0, t.sxx, 1e-6);
    ASSERT_NEAR(-1.0, t.syy, 1e-6);
    ASSERT_NEAR(-0.9, t.szz, 1e-6);
}

TEST_F(MarkeredBoxMesh, moveMarkers)
{
    auto& mg = gcm::MarkeredBoxMeshGenerator::getInstance();

    MarkeredMesh m;
    mg.loadMesh(&m, nullptr, 4.0, 2.0, 4);

    m.reconstructBorder();
    m.markUnusedNodes();

    float *x = new float[m.getMarkers().size()];
    float *y = new float[m.getMarkers().size()];
    float *z = new float[m.getMarkers().size()];

    int i = 0;
    for (auto _m : m.getMarkers()) {
        x[i] = _m.x;
        y[i] = _m.y;
        z[i] = _m.z;
        i++;
    }

    for (i = 0; i < m.getNodesNumber(); i++) {
        auto& node = m.getNodeByLocalIndex(i);
        node.vx = 0.1;
        node.vy = 0.2;
        node.vz = 0.3;
    }

    m.moveMarkers(1);

    i = 0;
    for (auto _m : m.getMarkers()) {
        ASSERT_FLOAT_EQ(x[i] + 0.1, _m.x);
        ASSERT_FLOAT_EQ(y[i] + 0.2, _m.y);
        ASSERT_FLOAT_EQ(z[i] + 0.3, _m.z);
        i++;
    }

    delete[] x;
    delete[] y;
    delete[] z;
}*/


TEST(MarkeredSurfaceGeoGenerator, Sphere)
{
    auto& gen = gcm::MarkeredSurfaceGeoGenerator::getInstance();
    auto surface = gen.generate("models/sphere.geo");

    float dh = 1e-4;

    AABB outer = {-1-dh, 1+dh, -1-dh, 1+dh, -1-dh, 1+dh};
    AABB inner = {-1+dh, 1-dh, -1+dh, 1-dh, -1+dh, 1-dh};

    ASSERT_TRUE(outer.includes(surface.getAABB()));
    ASSERT_TRUE(surface.getAABB().includes(inner));


    gcm::real center[] = {0, 0, 0};

    const auto& nodes = surface.getMarkerNodes();
    
    for (auto m: nodes)
    {
        auto dist = distance(center, m.coords);
        EXPECT_NEAR(1.0, dist, 1e-4);
    }
    
    vector3r norm;
    
    for (auto f: surface.getMarkerFaces())
    {
        findTriangleFaceNormal(nodes[f.verts[0]].coords, nodes[f.verts[1]].coords, nodes[f.verts[2]].coords, &norm.x, &norm.y, &norm.z);
        vector3r r(nodes[f.verts[0]].x, nodes[f.verts[0]].y, nodes[f.verts[0]].z);
        
        EXPECT_NEAR(norm*r, 1.0, 1e-2);
    }
}

TEST(MarkeredSurfaceGeoGenerator, Cube)
{
    auto& gen = gcm::MarkeredSurfaceGeoGenerator::getInstance();
    auto surface = gen.generate("models/cube.geo");


    float dh = 1e-4;

    AABB outer = {-5-dh, 5+dh, -5-dh, 5+dh, -5-dh, 5+dh};
    AABB inner = {-5+dh, 5-dh, -5+dh, 5-dh, -5+dh, 5-dh};

    ASSERT_TRUE(outer.includes(surface.getAABB()));
    ASSERT_TRUE(surface.getAABB().includes(inner));
    
    const auto& nodes = surface.getMarkerNodes();

    for (auto m: nodes)
    {
        if (fabs(fabs(m.z)-5) < EQUALITY_TOLERANCE)
        {
            EXPECT_LT(m.x, 5.0+EQUALITY_TOLERANCE);
            EXPECT_GT(m.x, -5.0-EQUALITY_TOLERANCE);
            EXPECT_LT(m.y, 5.0+EQUALITY_TOLERANCE);
            EXPECT_GT(m.y, -5.0-EQUALITY_TOLERANCE);
        }
        else if (fabs(fabs(m.y)-5) < EQUALITY_TOLERANCE)
        {
            EXPECT_LT(m.x, 5.0+EQUALITY_TOLERANCE);
            EXPECT_GT(m.x, -5.0-EQUALITY_TOLERANCE);
            EXPECT_LT(m.z, 5.0+EQUALITY_TOLERANCE);
            EXPECT_GT(m.z, -5.0-EQUALITY_TOLERANCE);
        }
        else if (fabs(fabs(m.x)-5) < EQUALITY_TOLERANCE)
        {
            EXPECT_LT(m.y, 5.0+EQUALITY_TOLERANCE);
            EXPECT_GT(m.y, -5.0-EQUALITY_TOLERANCE);
            EXPECT_LT(m.z, 5.0+EQUALITY_TOLERANCE);
            EXPECT_GT(m.z, -5.0-EQUALITY_TOLERANCE);
        }
        else
            ASSERT_TRUE(false);

    }
    
    vector3r norm, _norm;
    
    for (auto f: surface.getMarkerFaces())
    {
        findTriangleFaceNormal(nodes[f.verts[0]].coords, nodes[f.verts[1]].coords, nodes[f.verts[2]].coords, &norm.x, &norm.y, &norm.z);
        
        gcm::real _x = (nodes[f.verts[0]].x + nodes[f.verts[1]].x + nodes[f.verts[2]].x) / 3;
        gcm::real _y = (nodes[f.verts[0]].y + nodes[f.verts[1]].y + nodes[f.verts[2]].y) / 3;
        gcm::real _z = (nodes[f.verts[0]].z + nodes[f.verts[1]].z + nodes[f.verts[2]].z) / 3;
        
        if (fabs(fabs(_z)-5) < EQUALITY_TOLERANCE)
        {
            _norm.x = _norm.y = 0;
            _norm.z = sgn(_z);
        }
        else if (fabs(fabs(_y)-5) < EQUALITY_TOLERANCE)
        {
            _norm.x = _norm.z = 0;
            _norm.y = sgn(_y);            
        }
        else if (fabs(fabs(_x)-5) < EQUALITY_TOLERANCE)
        {
            _norm.y = _norm.z = 0;
            _norm.x = sgn(_x);            
        }
        else
            ASSERT_TRUE(false);
        
        EXPECT_NEAR(norm*_norm, 1.0, 1e-2);
    }
    
}

TEST(MarkeredMeshNGGeometry, Sphere)
{
    auto& gen = gcm::MarkeredSurfaceGeoGenerator::getInstance();
    auto surface = gen.generate("models/sphere.geo");

    ASSERT_NO_THROW(
        MarkeredMeshNG mesh(surface, 100);
        mesh.preProcess();
    );

}

TEST(MarkeredMeshNGGeometry, Cube)
{
    auto& gen = gcm::MarkeredSurfaceGeoGenerator::getInstance();
    auto surface = gen.generate("models/cube.geo");

    ASSERT_NO_THROW(
        MarkeredMeshNG mesh(surface, 100);    
        mesh.preProcess();
    );
}

TEST(MarkeredMeshNG, getCellIndexes)
{
    auto& gen = gcm::MarkeredSurfaceGeoGenerator::getInstance();
    auto surface = gen.generate("models/cube.geo");
    MarkeredMeshNG mesh(surface, 100);
    
    for (uint i = 0; i < 100; i++)
        for (uint j = 0; j < 100; j++)
            for (uint k = 0; k < 100; k++)
            {
                auto idx = mesh.getCellLocalIndex(i, j, k);
                uint _i, _j, _k;
                
                mesh.getCellIndexes(idx, _i, _j, _k);
                
                ASSERT_EQ(i, _i);
                ASSERT_EQ(j, _j);
                ASSERT_EQ(k, _k);
            }
}
