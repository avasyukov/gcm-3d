#include <gtest/gtest.h>

#include "launcher/util/xml.hpp"
#include "launcher/loaders/material/MaterialLoader.hpp"
#include "libgcm/Engine.hpp"
#include "libgcm/rheology/Material.hpp"

using namespace launcher;
using namespace std;
using namespace gcm;

TEST(MaterialLoader, ValidAnisotropic)
{
    auto& loader = MaterialLoader::getInstance();

    auto doc = xml::Doc::fromString("                                 \
    <materials>                                                       \
            <material name=\"testMaterial\" rheology=\"anisotropic\"> \
                <crackThreshold>1.7</crackThreshold>                  \
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

    ASSERT_FLOAT_EQ(1.7, material->getCrackThreshold());
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
                <crackThreshold>1.7</crackThreshold>                  \
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

    auto sourceMaterial = makeMaterialPtr("sourceMaterial", 1.0, 1.0, props);

    Engine::getInstance().addMaterial(sourceMaterial);

    ASSERT_NO_THROW(
        material = loader.load(doc.getRootElement().getChildByName("material"));
    );

    // FIXME
    // set correct expected values below
    const auto& p = material->getRheologyProperties();

    ASSERT_FLOAT_EQ(p.c11, 0.0);
    ASSERT_FLOAT_EQ(p.c12, 0.0);
    ASSERT_FLOAT_EQ(p.c13, 0.0);
    ASSERT_FLOAT_EQ(p.c14, 0.0);
    ASSERT_FLOAT_EQ(p.c15, 0.0);
    ASSERT_FLOAT_EQ(p.c16, 0.0);
    ASSERT_FLOAT_EQ(p.c22, 0.0);
    ASSERT_FLOAT_EQ(p.c23, 0.0);
    ASSERT_FLOAT_EQ(p.c24, 0.0);
    ASSERT_FLOAT_EQ(p.c25, 0.0);
    ASSERT_FLOAT_EQ(p.c26, 0.0);
    ASSERT_FLOAT_EQ(p.c33, 0.0);
    ASSERT_FLOAT_EQ(p.c34, 0.0);
    ASSERT_FLOAT_EQ(p.c35, 0.0);
    ASSERT_FLOAT_EQ(p.c36, 0.0);
    ASSERT_FLOAT_EQ(p.c44, 0.0);
    ASSERT_FLOAT_EQ(p.c45, 0.0);
    ASSERT_FLOAT_EQ(p.c46, 0.0);
    ASSERT_FLOAT_EQ(p.c55, 0.0);
    ASSERT_FLOAT_EQ(p.c56, 0.0);
    ASSERT_FLOAT_EQ(p.c66, 0.0);
}

TEST(MaterialLoader, IsotropicValid)
{
    auto& loader = MaterialLoader::getInstance();

    auto doc = xml::Doc::fromString("                               \
    <materials>                                                     \
            <material name=\"testMaterial\" rheology=\"isotropic\"> \
                <crackThreshold>1.5</crackThreshold>                \
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

    ASSERT_FLOAT_EQ(1.5, material->getCrackThreshold());
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
                <crackThreshold>0</crackThreshold>                  \
                <la>70000</la>                                      \
                <mu>XXXX</mu>                                       \
                <rho>1</rho>                                        \
            </material>                                             \
    </materials>                                                    \
    ");

    ASSERT_ANY_THROW(loader.load(doc.getRootElement().getChildByName("material")));
}
