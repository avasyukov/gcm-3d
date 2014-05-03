#include <gtest/gtest.h>

#include "libgcm/rheology/Material.hpp"
#include "libgcm/rheology/RheologyMatrix.hpp"
#include "libgcm/rheology/setters/IsotropicRheologyMatrixSetter.hpp"
#include "libgcm/rheology/decomposers/IsotropicRheologyMatrixDecomposer.hpp"
#include "libgcm/node/CalcNode.hpp"


using namespace gcm;

TEST(Rheology, IsotropicParameters)
{
    Material::RheologyProperties params = {100, 300};

    ASSERT_FLOAT_EQ(params.getLa(), 100);
    ASSERT_FLOAT_EQ(params.getMu(), 300);
}

TEST(Rheology, IsotropicMaterial)
{
    MaterialPtr material = makeMaterialPtr("test", 1, 1e20, 100, 300);

    ASSERT_FLOAT_EQ(material->getLa(), 100);
    ASSERT_FLOAT_EQ(material->getMu(), 300);
}

