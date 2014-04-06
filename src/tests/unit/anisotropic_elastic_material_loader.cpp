#include <gtest/gtest.h>

#include "launcher/loaders/material/AnisotropicElasticMaterialLoader.hpp"
#include "launcher/util/xml.hpp"
#include "libgcm/materials/IAnisotropicElasticMaterial.hpp"
#include "libgcm/materials/AnisotropicElasticMaterial.hpp"

using namespace launcher;
using namespace std;

TEST(AnisotropicElasticMaterialLoader, Valid)
{
    auto& loader = AnisotropicElasticMaterialLoader::getInstance();

    auto doc = xml::Doc::fromString("                                        \
    <materials>                                                              \
            <material name=\"testMaterial\" rheology=\"AnisotropicElastic\"> \
                <crackThreshold>1.7</crackThreshold>                         \
                <c11>1</c11>                                                 \
                <c12>2</c12>                                                 \
                <c13>3</c13>                                                 \
                <c14>4</c14>                                                 \
                <c15>5</c15>                                                 \
                <c16>6</c16>                                                 \
                <c22>7</c22>                                                 \
                <c23>8</c23>                                                 \
                <c24>9</c24>                                                 \
                <c25>10</c25>                                                \
                <c26>11</c26>                                                \
                <c33>12</c33>                                                \
                <c34>13</c34>                                                \
                <c35>14</c35>                                                \
                <c36>15</c36>                                                \
                <c44>16</c44>                                                \
                <c45>17</c45>                                                \
                <c46>18</c46>                                                \
                <c55>19</c55>                                                \
                <c56>20</c56>                                                \
                <c66>21</c66>                                                \
                <rho>1</rho>                                                 \
            </material>                                                      \
    </materials>                                                             \
    ");

    AnisotropicElasticMaterial* material = nullptr;
    ASSERT_NO_THROW(
        material = loader.load(doc.getRootElement().getChildByName("material"));
    );

    ASSERT_FLOAT_EQ(1.7, material->getCrackThreshold());
    ASSERT_FLOAT_EQ(1, material->getRho());

    for (int i = 0; i < ANISOTROPIC_ELASTIC_MATERIALS_PARAMETERS_NUM; i++ )
        ASSERT_FLOAT_EQ(material->getParameters().values[i], i+1);

}

TEST(AnisotropicElasticMaterialLoader, Empty)
{
    auto& loader = AnisotropicElasticMaterialLoader::getInstance();

    auto doc = xml::Doc::fromString("                  \
    <materials>                                        \
            <material rheology=\"AnisotropicElastic\"> \
            </material>                                \
    </materials>                                       \
    ");
    ASSERT_ANY_THROW(loader.load(doc.getRootElement().getChildByName("material")));
}

TEST(AnisotropicElasticMaterialLoader, Invalid)
{
    auto& loader = AnisotropicElasticMaterialLoader::getInstance();

    auto doc = xml::Doc::fromString("                                        \
    <materials>                                                              \
            <material name=\"testMaterial\" rheology=\"AnisotropicElastic\"> \
                <crackThreshold>1.7</crackThreshold>                         \
                <c11>1</c11>                                                 \
                <c12>2</c12>                                                 \
                <c13>3</c13>                                                 \
                <c14>4</c14>                                                 \
                <c15>5</c15>                                                 \
                <c16>6</c16>                                                 \
                <c22>7</c22>                                                 \
                <c23>8</c23>                                                 \
                <c24>9</c24>                                                 \
                <c25>10</c25>                                                \
                <c26>11</c26>                                                \
                <c33>12</c33>                                                \
                <c34>13</c34>                                                \
                <c35>14</c35>                                                \
                <c36>15</c36>                                                \
                <c44>16</c44>                                                \
                <c45>17</c45>                                                \
                <c46>18</c46>                                                \
                <c55>19</c55>                                                \
                <c56>20</c56>                                                \
                <c66>ZZ</c66>                                                \
                <rho>1</rho>                                                 \
            </material>                                                      \
    </materials>                                                             \
    ");

    ASSERT_ANY_THROW(
        loader.load(doc.getRootElement().getChildByName("material"))
    );
}

TEST(AnisotropicElasticMaterialLoader, Rotation)
{
    auto& loader = AnisotropicElasticMaterialLoader::getInstance();

    auto doc = xml::Doc::fromString("                                                             \
    <materials>                                                                                   \
            <material name=\"rotated\" rheology=\"AnisotropicElastic\" source=\"sourceMaterial\"> \
                <rotate a1=\"90\" a2=\"90\" a3=\"90\"/>                                           \
            </material>                                                                           \
    </materials>                                                                                  \
    ");

    AnisotropicElasticMaterial* material = nullptr;

    IAnisotropicElasticMaterial::RheologyParameters params;

    for (int i = 0; i < ANISOTROPIC_ELASTIC_MATERIALS_PARAMETERS_NUM; i++)
        params.values[i] = i*3;

    AnisotropicElasticMaterial sourceMaterial("sourceMaterial", 1.0, 1.0, params);

    Engine::getInstance().addMaterial(&sourceMaterial);

    ASSERT_NO_THROW(
        material = loader.load(doc.getRootElement().getChildByName("material"));
    );

    // FIXME
    // set correct expected values below
    const auto& p = material->getParameters();

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
