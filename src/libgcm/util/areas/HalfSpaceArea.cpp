#include "libgcm/util/areas/HalfSpaceArea.hpp"

using namespace gcm;

HalfSpaceArea::HalfSpaceArea( float _x, float _y, float _z, float _nx, float _ny, float _nz )
{
    x = _x;
    y = _y;
    z = _z;
    nx = _nx;
    ny = _ny;
    nz = _nz;
};

HalfSpaceArea::~HalfSpaceArea() {

}

bool HalfSpaceArea::isInArea( const Node& cur_node ) const
{
    return (    (cur_node.coords[0] - x) * nx +
                (cur_node.coords[1] - y) * ny +
                (cur_node.coords[2] - z) * nz > 0 );
};
