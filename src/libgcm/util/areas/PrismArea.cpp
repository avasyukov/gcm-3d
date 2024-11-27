#include "libgcm/util/areas/PrismArea.hpp"
#include "eigen3/Eigen/Core"
#include "eigen3/Eigen/LU"

using namespace gcm;

PrismArea::PrismArea( float p0[3], float p1[3], float p2[3], float p3[3] )
{
    for (int i = 0; i < 3; i++)
    {
        Points[0][i] = p0[i];
        Points[1][i] = p1[i];
        Points[2][i] = p2[i];
        Points[3][i] = p3[i];
    }
};

bool PrismArea::isInArea( const Node& cur_node ) const
{
    Eigen::Matrix3f bottomBase, topBase, swanSide, lobsterSide, pikeSide;
    bottomBase << cur_node.coords[0] - Points[0][0], cur_node.coords[1] - Points[0][1], cur_node.coords[2] - Points[0][2],
                  Points[1][0] - Points[0][0], Points[1][1] - Points[0][1], Points[1][2] - Points[0][2],
                  Points[2][0] - Points[0][0], Points[2][1] - Points[0][1], Points[2][2] - Points[0][2];
    topBase << cur_node.coords[0] - Points[3][0], cur_node.coords[1] - Points[3][1], cur_node.coords[2] - Points[3][2],
               Points[2][0] - Points[0][0], Points[2][1] - Points[0][1], Points[2][2] - Points[0][2],
               Points[1][0] - Points[0][0], Points[1][1] - Points[0][1], Points[1][2] - Points[0][2];
    swanSide << cur_node.coords[0] - Points[1][0], cur_node.coords[1] - Points[1][1], cur_node.coords[2] - Points[1][2],
                  Points[0][0] - Points[1][0], Points[0][1] - Points[1][1], Points[0][2] - Points[1][2],
                  Points[3][0] - Points[1][0], Points[3][1] - Points[1][1], Points[3][2] - Points[1][2];
    lobsterSide << cur_node.coords[0] - Points[0][0], cur_node.coords[1] - Points[0][1], cur_node.coords[2] - Points[0][2],
                   Points[2][0] - Points[0][0], Points[2][1] - Points[0][1], Points[2][2] - Points[0][2],
                   Points[3][0] - Points[0][0], Points[3][1] - Points[0][1], Points[3][2] - Points[0][2];
    pikeSide << cur_node.coords[0] - Points[1][0], cur_node.coords[1] - Points[1][1], cur_node.coords[2] - Points[1][2],
                  Points[3][0] - Points[0][0], Points[3][1] - Points[0][1], Points[3][2] - Points[0][2],
                  Points[2][0] - Points[1][0], Points[2][1] - Points[1][1], Points[2][2] - Points[1][2];
    return (bottomBase.determinant() > 0) && (topBase.determinant() > 0) && (swanSide.determinant() > 0)
           && (lobsterSide.determinant() > 0) && (pikeSide.determinant() > 0);
};
