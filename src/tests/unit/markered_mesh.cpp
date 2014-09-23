#include <cmath>
#include <gtest/gtest.h>

#include "libgcm/mesh/markers/MarkeredMesh.hpp"
#include "libgcm/mesh/markers/MarkeredSurfaceGeoGenerator.hpp"
#include "libgcm/Math.hpp"

#include "libgcm/snapshot/VTKMarkeredMeshSnapshotWriter.hpp"

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

TEST(MarkeredMeshGeometry, Sphere)
{
    auto& gen = gcm::MarkeredSurfaceGeoGenerator::getInstance();
    auto surface = gen.generate("models/sphere.geo");

    ASSERT_NO_THROW(
        MarkeredMesh mesh(surface, 100);
        mesh.preProcess();
    );

}

TEST(MarkeredMeshGeometry, Cube)
{
    auto& gen = gcm::MarkeredSurfaceGeoGenerator::getInstance();
    auto surface = gen.generate("models/cube.geo");

    ASSERT_NO_THROW(
        MarkeredMesh mesh(surface, 100);    
        mesh.preProcess();
    );
}

TEST(MarkeredMesh, getCellIndexes)
{
    auto& gen = gcm::MarkeredSurfaceGeoGenerator::getInstance();
    auto surface = gen.generate("models/cube.geo");
    MarkeredMesh mesh(surface, 100);
    
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
