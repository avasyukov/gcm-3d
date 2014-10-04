#include <gtest/gtest.h>

#include "libgcm/mesh/euler/EulerMesh.hpp"
#include "libgcm/mesh/euler/markers/MarkeredMesh.hpp"
#include "libgcm/mesh/euler/markers/MarkeredSurfaceGeoGenerator.hpp"
#include "libgcm/rheology/decomposers/IsotropicRheologyMatrixDecomposer.hpp"
#include "libgcm/rheology/setters/IsotropicRheologyMatrixSetter.hpp"
#include "libgcm/node/CalcNode.hpp"

class EulerMesh: public ::testing::Test
{
protected:

    static void SetUpTestCase()
    {
        gcm::Engine::getInstance().addMaterial(gcm::makeMaterialPtr("default", 0, 0, 0));
    }

};

using gcm::vector3u;
using gcm::vector3r;
using gcm::real;
using gcm::CalcNode;
using gcm::Exception;
using gcm::Engine;
using gcm::RheologyMatrixPtr;
using gcm::IsotropicRheologyMatrixDecomposer;
using gcm::IsotropicRheologyMatrixSetter;


class EulerMeshImpl: public gcm::EulerMesh
{
 public:
    EulerMeshImpl(vector3u dimensions, vector3r cellSize, vector3r center = {0, 0, 0}): EulerMesh(dimensions, cellSize, center)
    {
    }
    void preProcessGeometry() override
    {
    }
    void checkTopology(float tau) override
    {
    }
    void logMeshStats() override
    {
    }
};

#ifdef CONFIG_ENABLE_ASSERTIONS
TEST_F(EulerMesh, genearteMeshInvalidDimensions)
{

    ASSERT_THROW(
        EulerMeshImpl mesh({0, 1, 1}, {0.1, 0.2, 0.3}),
        Exception
    );

    ASSERT_THROW(
        EulerMeshImpl mesh({1, 0, 1}, {0.1, 0.2, 0.3}),
        Exception
    );
    ASSERT_THROW(
        EulerMeshImpl mesh({1, 1, 0}, {0.1, 0.2, 0.3}),
        Exception
    );
}

TEST_F(EulerMesh, genearteMeshInvalidCellSize)
{

    ASSERT_THROW(
        EulerMeshImpl mesh({1, 1, 1}, {0.0, 0.2, 0.3}),
        Exception
    );

    ASSERT_THROW(
        EulerMeshImpl mesh({1, 0, 1}, {0.1, 0.0, 0.3}),
        Exception
    );
    ASSERT_THROW(
        EulerMeshImpl mesh({1, 1, 0}, {0.1, 0.2, 0.0}),
        Exception
    );
}
#endif

TEST_F(EulerMesh, genearteMesh)
{

    EulerMeshImpl mesh({10, 20, 30}, {0.1, 0.2, 0.3}, {0.5, 2.0, 4.5});

    auto& node1 = mesh.getNodeByLocalIndex(0);
    auto& node2 = mesh.getNodeByLocalIndex(mesh.getNodesNumber()-1);

    ASSERT_FLOAT_EQ(0.0, node1.coords.x);
    ASSERT_FLOAT_EQ(0.0, node1.coords.y);
    ASSERT_FLOAT_EQ(0.0, node1.coords.z);

    ASSERT_FLOAT_EQ(1.0, node2.coords.x);
    ASSERT_FLOAT_EQ(4.0, node2.coords.y);
    ASSERT_FLOAT_EQ(9.0, node2.coords.z);
}

TEST_F(EulerMesh, getNodeMeshLocalIndexes)
{
    EulerMeshImpl mesh({10, 20, 30}, {0.1, 0.2, 0.3}, {0.5, 2.0, 4.5});

    for (uint i = 0; i <= 10; i++)
        for (uint j = 0; j <= 20; j++)
            for (uint k = 0; k <= 30; k++)
            {
                auto idx = mesh.getNodeLocalIndexByEulerMeshIndex(vector3u(i, j, k));

                vector3u index;

                ASSERT_TRUE(
                    mesh.getNodeEulerMeshIndex(idx, index)
                );

                ASSERT_EQ(i, index.x);
                ASSERT_EQ(j, index.y);
                ASSERT_EQ(k, index.z);
            }
}

TEST_F(EulerMesh, interpolateNode)
{
    EulerMeshImpl mesh({10, 10, 10}, {0.1, 0.1, 0.1});

    auto& engine = Engine::getInstance();
    auto mat = engine.getMaterial("default");
    auto setter = gcm::makeSetterPtr<IsotropicRheologyMatrixSetter>();
    auto decomposer = gcm::makeDecomposerPtr<IsotropicRheologyMatrixDecomposer>();
    auto rm = makeRheologyMatrixPtr(mat, setter, decomposer);

    for (int i = 0; i < mesh.getNodesNumber(); i++) {
        auto& node = mesh.getNodeByLocalIndex(i);
        node.setRheologyMatrix(rm);
        node.sxx = node.coords.x;
        node.syy = node.coords.y;
        node.szz = node.coords.z;
    }

    auto& n1 = mesh.getNodeByEulerMeshIndex({0, 0, 0});
    CalcNode r;
    bool inner;

    ASSERT_TRUE(
        static_cast<gcm::EulerMesh&>(mesh).interpolateNode(n1, 0.05, 0.0, 0.0, false, r, inner)
    );

    ASSERT_NEAR(r.coords.x, -0.45, EQUALITY_TOLERANCE);
    ASSERT_NEAR(r.coords.y, -0.50, EQUALITY_TOLERANCE);
    ASSERT_NEAR(r.coords.z, -0.50, EQUALITY_TOLERANCE);

    ASSERT_NEAR(r.sxx, -0.45, EQUALITY_TOLERANCE);
    ASSERT_NEAR(r.syy, -0.50, EQUALITY_TOLERANCE);
    ASSERT_NEAR(r.szz, -0.50, EQUALITY_TOLERANCE);

    ASSERT_FALSE(
        static_cast<gcm::EulerMesh&>(mesh).interpolateNode(n1, -0.05, 0.0, 0.0, false, r, inner)
    );
}

/*
 * This test actually uses MarkeredMesh instead of EulerMesh because we need some preparation
 * to be done before EulerMesh::interpolateBorderNode may be called.
 */
TEST_F(EulerMesh, interpolateBorderNode)
{
    auto& gen = gcm::MarkeredSurfaceGeoGenerator::getInstance();
    auto surface = gen.generate("models/cube.geo");

    gcm::MarkeredMesh mesh(surface, {100, 100, 100}, {0.12, 0.12, 0.12});

    auto& engine = Engine::getInstance();
    auto mat = engine.getMaterial("default");
    auto setter = gcm::makeSetterPtr<IsotropicRheologyMatrixSetter>();
    auto decomposer = gcm::makeDecomposerPtr<IsotropicRheologyMatrixDecomposer>();
    auto rm = makeRheologyMatrixPtr(mat, setter, decomposer);

    for (int i = 0; i < mesh.getNodesNumber(); i++) {
        auto& node = mesh.getNodeByLocalIndex(i);
        node.setRheologyMatrix(rm);
    }

    mesh.reconstructBorder();

    CalcNode node;

    ASSERT_TRUE(
        mesh.interpolateBorderNode(-5.1, -4.75, -4.75, 0.3, 0.0, 0.0, node)
    );
    ASSERT_TRUE(
        mesh.interpolateBorderNode(5.1, -4.75, -4.75, -0.3, 0.0, 0.0, node)
    );
    ASSERT_FALSE(
        mesh.interpolateBorderNode(-5.1, -4.75, -4.75, 0.03, 0.0, 0.0, node)
    );
    ASSERT_FALSE(
        mesh.interpolateBorderNode(5.1, -4.75, -4.75, -0.03, 0.0, 0.0, node)
    );

    ASSERT_TRUE(
        mesh.interpolateBorderNode(-4.75, -5.1, -4.75, 0.0, 0.3, 0.0, node)
    );
    ASSERT_TRUE(
        mesh.interpolateBorderNode(-4.75, 5.1, -4.75, 0.0, -0.3, 0.0, node)
    );
    ASSERT_FALSE(
        mesh.interpolateBorderNode(-4.75, -5.1, -4.75, 0.0, 0.03, 0.0, node)
    );
    ASSERT_FALSE(
        mesh.interpolateBorderNode(-4.75, 5.1, -4.75, 0.0, -0.03, 0.0, node)
    );

    ASSERT_TRUE(
        mesh.interpolateBorderNode(-4.75, -4.75, -5.1, 0.0, 0.0, 0.3, node)
    );
    ASSERT_TRUE(
            mesh.interpolateBorderNode(-4.75, -4.75, 5.1, 0.0, 0.0, -0.3, node)
    );
    ASSERT_FALSE(
        mesh.interpolateBorderNode(-4.75, -4.75, -5.1, 0.0, 0.0, 0.03, node)
    );
    ASSERT_FALSE(
        mesh.interpolateBorderNode(-4.75, -4.75, 5.1, 0.0, 0.0, -0.03, node)
    );
}

TEST_F(EulerMesh, getCellEulerIndexByCoords)
{
    EulerMeshImpl mesh({10, 10, 10}, {0.1, 0.2, 0.3});

    auto index = mesh.getCellEulerIndexByCoords(vector3r(-0.49, -0.99, -1.49));

    ASSERT_EQ(index.x, 0);
    ASSERT_EQ(index.y, 0);
    ASSERT_EQ(index.z, 0);

    index = mesh.getCellEulerIndexByCoords(vector3r(-0.39, -0.79, -1.19));

    ASSERT_EQ(index.x, 1);
    ASSERT_EQ(index.y, 1);
    ASSERT_EQ(index.z, 1);

    index = mesh.getCellEulerIndexByCoords(vector3r(0.49, 0.99, 1.49));

    ASSERT_EQ(index.x, 9);
    ASSERT_EQ(index.y, 9);
    ASSERT_EQ(index.z, 9);

#ifdef CONFIG_ENABLE_ASSERTIONS
    ASSERT_THROW(
        mesh.getCellEulerIndexByCoords(vector3r(-1, -0.79, -1.19)),
        Exception
    );
#endif

}

TEST_F(EulerMesh, getCellCenter)
{
    EulerMeshImpl mesh({10, 10, 10}, {0.1, 0.2, 0.3});

    auto center = mesh.getCellCenter(vector3u(0, 0, 0));

    ASSERT_FLOAT_EQ(center.x, -0.45);
    ASSERT_FLOAT_EQ(center.y, -0.9);
    ASSERT_FLOAT_EQ(center.z, -1.35);

    center = mesh.getCellCenter(vector3u(9, 9, 9));

    ASSERT_FLOAT_EQ(center.x, 0.45);
    ASSERT_FLOAT_EQ(center.y, 0.9);
    ASSERT_FLOAT_EQ(center.z, 1.35);

#ifdef CONFIG_ENABLE_ASSERTIONS
    ASSERT_THROW(
        mesh.getCellCenter(vector3u(11, 0, 0)),
        Exception
    );
#endif
}
