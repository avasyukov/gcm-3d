#include <gtest/gtest.h>

#include "loaders/material/IsotropicElasticMaterialLoader.h"
#include "util/xml.h"

using namespace launcher;
using namespace std;

TEST(IsotropicElasticMaterialLoader, Valid)
{
    auto loader = IsotropicElasticMaterialLoader::getInstance();

    auto doc = xml::Doc::fromString("                     \
	<materials>                                       \
            <material name=\"testMaterial\" type=\"IEM\"> \
                <crackThreshold>1.5</crackThreshold>      \
                <la>70000</la>                            \
                <mu>10000</mu>                            \
                <rho>1</rho>                              \
            </material>                                   \
	</materials>                                      \
    ");

    auto material = loader.load(doc.getRootElement().getChildByName("material"));

    ASSERT_FLOAT_EQ(1.5, material->getCrackThreshold());
    ASSERT_FLOAT_EQ(1, material->getRho());
    ASSERT_FLOAT_EQ(70000, material->getLambda());
    ASSERT_FLOAT_EQ(10000, material->getMu());
}

TEST(IsotropicElasticMaterialLoader, Empty)
{
    auto loader = IsotropicElasticMaterialLoader::getInstance();

    auto doc = xml::Doc::fromString("    \
	<materials>                      \
            <material type=\"IEM\">      \
            </material>                  \
	</materials>                     \
    ");
    ASSERT_ANY_THROW(loader.load(doc.getRootElement().getChildByName("material")));
}

TEST(IsotropicElasticMaterialLoader, Invalid)
{
    auto loader = IsotropicElasticMaterialLoader::getInstance();

    auto doc = xml::Doc::fromString("                     \
	<materials>                                       \
            <material name=\"testMaterial\" type=\"IEM\"> \
                <crackThreshold>0</crackThreshold>        \
                <la>70000</la>                            \
                <mu>XXXX</mu>                             \
                <rho>1</rho>                              \
            </material>                                   \
	</materials>                                      \
    ");
    
    ASSERT_ANY_THROW(loader.load(doc.getRootElement().getChildByName("material")));
}

