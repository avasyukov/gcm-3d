#include <gtest/gtest.h>

#include "loaders/material/AnisotropicElasticMaterialLoader.h"
#include "materials/IAnisotropicElasticMaterial.h"
#include "util/xml.h"
#include "materials/AnisotropicElasticMaterial.h"

using namespace launcher;
using namespace std;

TEST(AnisotropicElasticMaterialLoader, Valid)
{
    auto loader = AnisotropicElasticMaterialLoader::getInstance();

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
    auto loader = AnisotropicElasticMaterialLoader::getInstance();

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
    auto loader = AnisotropicElasticMaterialLoader::getInstance();

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

