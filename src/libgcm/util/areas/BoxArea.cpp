#include "libgcm/util/areas/BoxArea.hpp"

using namespace gcm;

BoxArea::BoxArea( float _minX, float _maxX, float _minY, float _maxY, float _minZ, float _maxZ )
{
    minX = _minX;
    maxX = _maxX;
    minY = _minY;
    maxY = _maxY;
    minZ = _minZ;
    maxZ = _maxZ;
};

BoxArea::~BoxArea() {

}

bool BoxArea::isInArea( const Node& cur_node ) const
{
    return ( cur_node.coords[0] < maxX &&
                cur_node.coords[0] > minX &&
                cur_node.coords[1] < maxY &&
                cur_node.coords[1] > minY &&
                cur_node.coords[2] < maxZ &&
                cur_node.coords[2] > minZ );
};
