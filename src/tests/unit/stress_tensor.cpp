#include "libgcm/util/StressTensor.hpp"
#include "launcher/launcher.hpp"
#include "libgcm/rheology/decomposers/IsotropicRheologyMatrixDecomposer.hpp"
#include "libgcm/rheology/setters/IsotropicRheologyMatrixSetter.hpp"

#include <gtest/gtest.h>

using namespace gcm;
using namespace gcm::linal;

// FIXME why just using namespace is not enough?
using gcm::IsotropicRheologyMatrixDecomposer;
using gcm::IsotropicRheologyMatrixSetter;


TEST(StressTensor, Transform)
{
    StressTensor tensorX(
        -900.0,  0.0  ,  0.0  ,
                -700.0,  0.0  ,
                        -700.0
    );
    
    StressTensor tensorY(
        -700.0,  0.0  ,  0.0  ,
                -900.0,  0.0  ,
                        -700.0
    );

    StressTensor tensorZ(
        -700.0,  0.0  ,  0.0  ,
                -700.0,  0.0  ,
                        -900.0
    );

    Matrix33 sXY({
         0.0, 1.0, 0.0,
        -1.0, 0.0, 0.0,
         0.0, 0.0, 1.0
    });
    
    Matrix33 sZY({
        1.0, 0.0,  0.0,
        0.0, 0.0, -1.0,
        0.0, 1.0,  0.0
    });
    
    tensorX.transform(sXY);

    ASSERT_NEAR(tensorX.xx, tensorY.xx, 1e-5);
    ASSERT_NEAR(tensorX.xy, tensorY.xy, 1e-5);
    ASSERT_NEAR(tensorX.xz, tensorY.xz, 1e-5);
    ASSERT_NEAR(tensorX.yy, tensorY.yy, 1e-5);
    ASSERT_NEAR(tensorX.yz, tensorY.yz, 1e-5);
    ASSERT_NEAR(tensorX.zz, tensorY.zz, 1e-5);

    tensorZ.transform(sZY);

    ASSERT_NEAR(tensorZ.xx, tensorY.xx, 1e-5);
    ASSERT_NEAR(tensorZ.xy, tensorY.xy, 1e-5);
    ASSERT_NEAR(tensorZ.xz, tensorY.xz, 1e-5);
    ASSERT_NEAR(tensorZ.yy, tensorY.yy, 1e-5);
    ASSERT_NEAR(tensorZ.yz, tensorY.yz, 1e-5);
    ASSERT_NEAR(tensorZ.zz, tensorY.zz, 1e-5);
}


class StressTensorWaves: public ::testing::Test
{
protected:

    static void SetUpTestCase()
    {
        gcm::Engine::getInstance().addMaterial(gcm::makeMaterialPtr("StressTensorWavesMaterial", 1, 70000, 10000));
    }

};

TEST_F(StressTensorWaves, IsotropicElasticPWaveAlongX)
{
    CalcNode node;

    auto& engine = Engine::getInstance();
    auto mat = engine.getMaterial("StressTensorWavesMaterial");
    node.setMaterialId(engine.getMaterialIndex(mat->getName()));
    auto setter = makeSetterPtr<IsotropicRheologyMatrixSetter>();
    auto decomposer = makeDecomposerPtr<IsotropicRheologyMatrixDecomposer>();
    auto rm = makeRheologyMatrixPtr(mat, setter, decomposer);

    node.setRheologyMatrix(rm);

    launcher::setIsotropicElasticPWave(node, Vector3({1.0, 0.0, 0.0}), 0.01, true);

    ASSERT_NEAR(node.sxx, 900.0, 1e-5);
    ASSERT_NEAR(node.sxy, 0.0, 1e-5);
    ASSERT_NEAR(node.sxz, 0.0, 1e-5);
    ASSERT_NEAR(node.syy, 700.0, 1e-5);
    ASSERT_NEAR(node.syz, 0.0, 1e-5);
    ASSERT_NEAR(node.szz, 700.0, 1e-5);
    
    ASSERT_NEAR(node.vx, -3.0, 1e-5);
    ASSERT_NEAR(node.vy, 0.0, 1e-5);
    ASSERT_NEAR(node.vz, 0.0, 1e-5);
    
    launcher::setIsotropicElasticPWave(node, Vector3({1.0, 0.0, 0.0}), 0.01, false);

    ASSERT_NEAR(node.sxx, -900.0, 1e-5);
    ASSERT_NEAR(node.sxy, 0.0, 1e-5);
    ASSERT_NEAR(node.sxz, 0.0, 1e-5);
    ASSERT_NEAR(node.syy, -700.0, 1e-5);
    ASSERT_NEAR(node.syz, 0.0, 1e-5);
    ASSERT_NEAR(node.szz, -700.0, 1e-5);
    
    ASSERT_NEAR(node.vx, 3.0, 1e-5);
    ASSERT_NEAR(node.vy, 0.0, 1e-5);
    ASSERT_NEAR(node.vz, 0.0, 1e-5);
}

TEST_F(StressTensorWaves, IsotropicElasticPWaveAlongY)
{
    CalcNode node;

    auto& engine = Engine::getInstance();
    auto mat = engine.getMaterial("StressTensorWavesMaterial");
    node.setMaterialId(engine.getMaterialIndex(mat->getName()));
    auto setter = makeSetterPtr<IsotropicRheologyMatrixSetter>();
    auto decomposer = makeDecomposerPtr<IsotropicRheologyMatrixDecomposer>();
    auto rm = makeRheologyMatrixPtr(mat, setter, decomposer);

    node.setRheologyMatrix(rm);

    launcher::setIsotropicElasticPWave(node, Vector3({0.0, 1.0, 0.0}), 0.01, true);

    ASSERT_NEAR(node.sxx, 700.0, 1e-5);
    ASSERT_NEAR(node.sxy, 0.0, 1e-5);
    ASSERT_NEAR(node.sxz, 0.0, 1e-5);
    ASSERT_NEAR(node.syy, 900.0, 1e-5);
    ASSERT_NEAR(node.syz, 0.0, 1e-5);
    ASSERT_NEAR(node.szz, 700.0, 1e-5);
    
    ASSERT_NEAR(node.vx, 0.0, 1e-5);
    ASSERT_NEAR(node.vy, -3.0, 1e-5);
    ASSERT_NEAR(node.vz, 0.0, 1e-5);
    
    launcher::setIsotropicElasticPWave(node, Vector3({0.0, 1.0, 0.0}), 0.01, false);

    ASSERT_NEAR(node.sxx, -700.0, 1e-5);
    ASSERT_NEAR(node.sxy, 0.0, 1e-5);
    ASSERT_NEAR(node.sxz, 0.0, 1e-5);
    ASSERT_NEAR(node.syy, -900.0, 1e-5);
    ASSERT_NEAR(node.syz, 0.0, 1e-5);
    ASSERT_NEAR(node.szz, -700.0, 1e-5);
    
    ASSERT_NEAR(node.vx, 0.0, 1e-5);
    ASSERT_NEAR(node.vy, 3.0, 1e-5);
    ASSERT_NEAR(node.vz, 0.0, 1e-5);
}

TEST_F(StressTensorWaves, IsotropicElasticPWaveAlongZ)
{
    CalcNode node;

    auto& engine = Engine::getInstance();
    auto mat = engine.getMaterial("StressTensorWavesMaterial");
    node.setMaterialId(engine.getMaterialIndex(mat->getName()));
    auto setter = makeSetterPtr<IsotropicRheologyMatrixSetter>();
    auto decomposer = makeDecomposerPtr<IsotropicRheologyMatrixDecomposer>();
    auto rm = makeRheologyMatrixPtr(mat, setter, decomposer);

    node.setRheologyMatrix(rm);

    launcher::setIsotropicElasticPWave(node, Vector3({0.0, 0.0, 1.0}), 0.01, true);

    ASSERT_NEAR(node.sxx, 700.0, 1e-5);
    ASSERT_NEAR(node.sxy, 0.0, 1e-5);
    ASSERT_NEAR(node.sxz, 0.0, 1e-5);
    ASSERT_NEAR(node.syy, 700.0, 1e-5);
    ASSERT_NEAR(node.syz, 0.0, 1e-5);
    ASSERT_NEAR(node.szz, 900.0, 1e-5);
    
    ASSERT_NEAR(node.vx, 0.0, 1e-5);
    ASSERT_NEAR(node.vy, 0.0, 1e-5);
    ASSERT_NEAR(node.vz, -3.0, 1e-5);
    
    launcher::setIsotropicElasticPWave(node, Vector3({0.0, 0.0, 1.0}), 0.01, false);

    ASSERT_NEAR(node.sxx, -700.0, 1e-5);
    ASSERT_NEAR(node.sxy, 0.0, 1e-5);
    ASSERT_NEAR(node.sxz, 0.0, 1e-5);
    ASSERT_NEAR(node.syy, -700.0, 1e-5);
    ASSERT_NEAR(node.syz, 0.0, 1e-5);
    ASSERT_NEAR(node.szz, -900.0, 1e-5);

    ASSERT_NEAR(node.vx, 0.0, 1e-5);
    ASSERT_NEAR(node.vy, 0.0, 1e-5);
    ASSERT_NEAR(node.vz, 3.0, 1e-5);
}
