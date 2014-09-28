#include <cmath>
#include <gtest/gtest.h>

#include "libgcm/mesh/markers/MarkeredMesh.hpp"
#include "libgcm/mesh/markers/MarkeredSurfaceGeoGenerator.hpp"
#include "libgcm/Math.hpp"

#include "libgcm/snapshot/VTKMarkeredMeshSnapshotWriter.hpp"

using namespace gcm;

TEST(MarkeredSurfaceGeoGenerator, Sphere)
{
    auto& gen = MarkeredSurfaceGeoGenerator::getInstance();
    auto surface = gen.generate("models/sphere.geo");

    float dh = 1e-4;

    AABB outer = {-1-dh, 1+dh, -1-dh, 1+dh, -1-dh, 1+dh};
    AABB inner = {-1+dh, 1-dh, -1+dh, 1-dh, -1+dh, 1-dh};

    ASSERT_TRUE(outer.includes(surface.getAABB()));
    ASSERT_TRUE(surface.getAABB().includes(inner));


    real center[] = {0, 0, 0};

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

        EXPECT_NEAR(norm*nodes[f.verts[0]].coords, 1.0, 1e-2);
    }
}

TEST(MarkeredSurfaceGeoGenerator, Cube)
{
    auto& gen = MarkeredSurfaceGeoGenerator::getInstance();
    auto surface = gen.generate("models/cube.geo");


    float dh = 1e-4;

    AABB outer = {-5-dh, 5+dh, -5-dh, 5+dh, -5-dh, 5+dh};
    AABB inner = {-5+dh, 5-dh, -5+dh, 5-dh, -5+dh, 5-dh};

    ASSERT_TRUE(outer.includes(surface.getAABB()));
    ASSERT_TRUE(surface.getAABB().includes(inner));
    
    const auto& nodes = surface.getMarkerNodes();

    for (auto m: nodes)
    {
        if (fabs(fabs(m.coords.z)-5) < EQUALITY_TOLERANCE)
        {
            EXPECT_LT(m.coords.x, 5.0+EQUALITY_TOLERANCE);
            EXPECT_GT(m.coords.x, -5.0-EQUALITY_TOLERANCE);
            EXPECT_LT(m.coords.y, 5.0+EQUALITY_TOLERANCE);
            EXPECT_GT(m.coords.y, -5.0-EQUALITY_TOLERANCE);
        }
        else if (fabs(fabs(m.coords.y)-5) < EQUALITY_TOLERANCE)
        {
            EXPECT_LT(m.coords.x, 5.0+EQUALITY_TOLERANCE);
            EXPECT_GT(m.coords.x, -5.0-EQUALITY_TOLERANCE);
            EXPECT_LT(m.coords.z, 5.0+EQUALITY_TOLERANCE);
            EXPECT_GT(m.coords.z, -5.0-EQUALITY_TOLERANCE);
        }
        else if (fabs(fabs(m.coords.x)-5) < EQUALITY_TOLERANCE)
        {
            EXPECT_LT(m.coords.y, 5.0+EQUALITY_TOLERANCE);
            EXPECT_GT(m.coords.y, -5.0-EQUALITY_TOLERANCE);
            EXPECT_LT(m.coords.z, 5.0+EQUALITY_TOLERANCE);
            EXPECT_GT(m.coords.z, -5.0-EQUALITY_TOLERANCE);
        }
        else
            ASSERT_TRUE(false);

    }
    
    vector3r norm, _norm;
    
    for (auto f: surface.getMarkerFaces())
    {
        findTriangleFaceNormal(nodes[f.verts[0]].coords, nodes[f.verts[1]].coords, nodes[f.verts[2]].coords, &norm.x, &norm.y, &norm.z);
        
        auto r = (nodes[f.verts[0]].coords + nodes[f.verts[1]].coords + nodes[f.verts[2]].coords)/3;
        
        if (fabs(fabs(r.z)-5) < EQUALITY_TOLERANCE)
        {
            _norm.x = _norm.y = 0;
            _norm.z = sgn(r.z);
        }
        else if (fabs(fabs(r.y)-5) < EQUALITY_TOLERANCE)
        {
            _norm.x = _norm.z = 0;
            _norm.y = sgn(r.y);
        }
        else if (fabs(fabs(r.x)-5) < EQUALITY_TOLERANCE)
        {
            _norm.y = _norm.z = 0;
            _norm.x = sgn(r.x);
        }
        else
            ASSERT_TRUE(false);
        
        EXPECT_NEAR(norm*_norm, 1.0, 1e-2);
    }
    
}

TEST(MarkeredMeshGeometry, Sphere)
{
    auto& gen = MarkeredSurfaceGeoGenerator::getInstance();
    auto surface = gen.generate("models/sphere.geo");

    ASSERT_NO_THROW(
        MarkeredMesh mesh(surface, 100);
        mesh.preProcess();
    );

}

TEST(MarkeredMeshGeometry, Cube)
{
    auto& gen = MarkeredSurfaceGeoGenerator::getInstance();
    auto surface = gen.generate("models/cube.geo");

    ASSERT_NO_THROW(
        MarkeredMesh mesh(surface, 100);    
        mesh.preProcess();
    );
}

TEST(MarkeredMesh, getCellIndexes)
{
    auto& gen = MarkeredSurfaceGeoGenerator::getInstance();
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
