#include <gtest/gtest.h>

#include "launcher/util/xml.hpp"
#include "launcher/loaders/material/MaterialLoader.hpp"
#include "libgcm/Engine.hpp"
#include "libgcm/rheology/Material.hpp"

using namespace launcher;
using namespace gcm;

TEST(MaterialLoader, ValidAnisotropic)
{
    auto& loader = MaterialLoader::getInstance();

    auto doc = xml::Doc::fromString("                                 \
    <materials>                                                       \
            <material name=\"testMaterial\" rheology=\"anisotropic\"> \
                <c11>1</c11>                                          \
                <c12>2</c12>                                          \
                <c13>3</c13>                                          \
                <c14>4</c14>                                          \
                <c15>5</c15>                                          \
                <c16>6</c16>                                          \
                <c22>7</c22>                                          \
                <c23>8</c23>                                          \
                <c24>9</c24>                                          \
                <c25>10</c25>                                         \
                <c26>11</c26>                                         \
                <c33>12</c33>                                         \
                <c34>13</c34>                                         \
                <c35>14</c35>                                         \
                <c36>15</c36>                                         \
                <c44>16</c44>                                         \
                <c45>17</c45>                                         \
                <c46>18</c46>                                         \
                <c55>19</c55>                                         \
                <c56>20</c56>                                         \
                <c66>21</c66>                                         \
                <rho>1</rho>                                          \
                <plasticity type=\"1\">                               \
                    <param1>10</param1>                               \
                </plasticity>                                         \
                <plasticity type=\"2\">                               \
                    <param2>20</param2>                               \
                </plasticity>                                         \
            </material>                                               \
    </materials>                                                      \
    ");

    MaterialPtr material;
    ASSERT_NO_THROW(
        material = loader.load(doc.getRootElement().getChildByName("material"));
    );

    ASSERT_FALSE(material->isIsotropic());

    ASSERT_FLOAT_EQ(1, material->getRho());

    for (int i = 0; i < Material::RHEOLOGY_PROPERTIES_NUMBER; i++ )
        ASSERT_FLOAT_EQ(material->getRheologyProperties().values[i], i+1);
    auto props = material->getPlasticityProperties();
    ASSERT_EQ(props["1"]["param1"], 10.0);
    ASSERT_EQ(props["2"]["param2"], 20.0);

}

TEST(MaterialLoader, EmptyAnisotropic)
{
    auto& loader = MaterialLoader::getInstance();

    auto doc = xml::Doc::fromString("           \
    <materials>                                 \
            <material rheology=\"anisotropic\"> \
            </material>                         \
    </materials>                                \
    ");
    ASSERT_ANY_THROW(loader.load(doc.getRootElement().getChildByName("material")));
}

TEST(MaterialLoader, InvalidAnisotropic)
{
    auto& loader = MaterialLoader::getInstance();

    auto doc = xml::Doc::fromString("                                 \
    <materials>                                                       \
            <material name=\"testMaterial\" rheology=\"anisotropic\"> \
                <c11>1</c11>                                          \
                <c12>2</c12>                                          \
                <c13>3</c13>                                          \
                <c14>4</c14>                                          \
                <c15>5</c15>                                          \
                <c16>6</c16>                                          \
                <c22>7</c22>                                          \
                <c23>8</c23>                                          \
                <c24>9</c24>                                          \
                <c25>10</c25>                                         \
                <c26>11</c26>                                         \
                <c33>12</c33>                                         \
                <c34>13</c34>                                         \
                <c35>14</c35>                                         \
                <c36>15</c36>                                         \
                <c44>16</c44>                                         \
                <c45>17</c45>                                         \
                <c46>18</c46>                                         \
                <c55>19</c55>                                         \
                <c56>20</c56>                                         \
                <c66>ZZ</c66>                                         \
                <rho>1</rho>                                          \
            </material>                                               \
    </materials>                                                      \
    ");

    ASSERT_ANY_THROW(
        loader.load(doc.getRootElement().getChildByName("material"))
    );
}

TEST(MaterialLoader, AnisotropicRotation)
{
    auto& loader = MaterialLoader::getInstance();

    auto doc = xml::Doc::fromString("                                                      \
    <materials>                                                                            \
            <material name=\"rotated\" rheology=\"anisotropic\" source=\"sourceMaterial\"> \
                <rotate a1=\"90\" a2=\"90\" a3=\"90\"/>                                    \
            </material>                                                                    \
    </materials>                                                                           \
    ");

    MaterialPtr material;

    Material::RheologyProperties props;

    for (int i = 0; i < Material::RHEOLOGY_PROPERTIES_NUMBER; i++)
        props.values[i] = i*3;

    auto sourceMaterial = makeMaterialPtr("sourceMaterial", 1.0, props);

    Engine::getInstance().addMaterial(sourceMaterial);

    ASSERT_NO_THROW(
        material = loader.load(doc.getRootElement().getChildByName("material"));
    );

    const auto& p = material->getRheologyProperties();

    ASSERT_NEAR(p.c11, props.c33, EQUALITY_TOLERANCE);
    ASSERT_NEAR(p.c12, props.c32, EQUALITY_TOLERANCE);
    ASSERT_NEAR(p.c13, props.c31, EQUALITY_TOLERANCE);
    ASSERT_NEAR(p.c14, -props.c36, EQUALITY_TOLERANCE);
    ASSERT_NEAR(p.c15, -props.c35, EQUALITY_TOLERANCE);
    ASSERT_NEAR(p.c16, props.c34, EQUALITY_TOLERANCE);
    ASSERT_NEAR(p.c22, props.c22, EQUALITY_TOLERANCE);
    ASSERT_NEAR(p.c23, props.c21, EQUALITY_TOLERANCE);
    ASSERT_NEAR(p.c24, -props.c26, EQUALITY_TOLERANCE);
    ASSERT_NEAR(p.c25, -props.c25, EQUALITY_TOLERANCE);
    ASSERT_NEAR(p.c26, props.c24, EQUALITY_TOLERANCE);
    ASSERT_NEAR(p.c33, props.c11, EQUALITY_TOLERANCE);
    ASSERT_NEAR(p.c34, -props.c16, EQUALITY_TOLERANCE);
    ASSERT_NEAR(p.c35, -props.c15, EQUALITY_TOLERANCE);
    ASSERT_NEAR(p.c36, props.c14, EQUALITY_TOLERANCE);
    ASSERT_NEAR(p.c44, props.c66, EQUALITY_TOLERANCE);
    ASSERT_NEAR(p.c45, props.c56, EQUALITY_TOLERANCE);
    ASSERT_NEAR(p.c46, -props.c46, EQUALITY_TOLERANCE);
    ASSERT_NEAR(p.c55, props.c55, EQUALITY_TOLERANCE);
    ASSERT_NEAR(p.c56, -props.c45, EQUALITY_TOLERANCE);
    ASSERT_NEAR(p.c66, props.c44, EQUALITY_TOLERANCE);
}

TEST(MaterialLoader, IsotropicValid)
{
    auto& loader = MaterialLoader::getInstance();

    auto doc = xml::Doc::fromString("                               \
    <materials>                                                     \
            <material name=\"testMaterial\" rheology=\"isotropic\"> \
                <la>70000</la>                                      \
                <mu>10000</mu>                                      \
                <rho>1</rho>                                        \
            </material>                                             \
    </materials>                                                    \
    ");

    MaterialPtr material;
    
    ASSERT_NO_THROW(
        material = loader.load(doc.getRootElement().getChildByName("material"));
    );
    
    ASSERT_TRUE(material->isIsotropic());

    ASSERT_FLOAT_EQ(1, material->getRho());
    ASSERT_FLOAT_EQ(70000, material->getLa());
    ASSERT_FLOAT_EQ(10000, material->getMu());
}

TEST(MaterialLoader, IsotropicEmpty)
{
    auto& loader = MaterialLoader::getInstance();

    auto doc = xml::Doc::fromString("         \
    <materials>                               \
            <material rheology=\"isotropic\"> \
            </material>                       \
    </materials>                              \
    ");
    ASSERT_ANY_THROW(loader.load(doc.getRootElement().getChildByName("material")));
}

TEST(MaterialLoader, IsotropicInvalid)
{
    auto& loader = MaterialLoader::getInstance();

    auto doc = xml::Doc::fromString("                               \
    <materials>                                                     \
            <material name=\"testMaterial\" rheology=\"isotropic\"> \
                <la>70000</la>                                      \
                <mu>XXXX</mu>                                       \
                <rho>1</rho>                                        \
            </material>                                             \
    </materials>                                                    \
    ");

    ASSERT_ANY_THROW(loader.load(doc.getRootElement().getChildByName("material")));
}
