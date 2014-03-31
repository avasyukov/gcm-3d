#include "libgcm/util/AABB.hpp"

gcm::AABB::AABB()
{
    minX = minY = minZ = numeric_limits<float>::infinity();
    maxX = maxY = maxZ = - numeric_limits<float>::infinity();
}

gcm::AABB::AABB(float _minX, float _maxX, float _minY, float _maxY, float _minZ, float _maxZ)
{
    minX = _minX;
    minY = _minY;
    minZ = _minZ;
    maxX = _maxX;
    maxY = _maxY;
    maxZ = _maxZ;
}

bool gcm::AABB::isInAABB( float x, float y, float z )
{
    // FIXME do we really have to check our state all the time?
    assert( minX != maxX && minY != maxY && minZ != maxZ );
    return ( x < maxX + EQUALITY_TOLERANCE
                && x > minX - EQUALITY_TOLERANCE
                && y < maxY + EQUALITY_TOLERANCE
                && y > minY - EQUALITY_TOLERANCE
                && z < maxZ + EQUALITY_TOLERANCE
                && z > minZ - EQUALITY_TOLERANCE );
};

bool gcm::AABB::isInAABB( Node* node )
{
    return isInAABB(node->coords[0], node->coords[1], node->coords[2]);
};

bool gcm::AABB::isInAABB( Node& node )
{
    return isInAABB(node.coords[0], node.coords[1], node.coords[2]);
};

bool gcm::AABB::includes(AABB* box)
{
    return ( minX <= box->minX ) && ( minY <= box->minY ) && ( minZ <= box->minZ )
            && ( maxX >= box->maxX ) && ( maxY >= box->maxY ) && ( maxZ >= box->maxZ );
}

bool gcm::AABB::intersects(AABB* box)
{
    float int_min_coords[3];
    float int_max_coords[3];
    // check for intersection
    for(int j = 0; j < 3; j++) {
        int_min_coords[j] = fmaxf(min_coords[j], box->min_coords[j]);
        int_max_coords[j] = fminf(max_coords[j], box->max_coords[j]);
        if(int_min_coords[j] > int_max_coords[j])
        {
            return false;
        }
    }
    return true;
}

/*bool gcm::AABB::intersects(AABB box)
{
    AABB* inters = findIntersection(&box);
    if(inters == NULL)
        return false;
    delete inters;
    return true;
}*/

AABB* gcm::AABB::findIntersection(AABB* box)
{
    float int_min_coords[3];
    float int_max_coords[3];
    // check for intersection
    for(int j = 0; j < 3; j++) {
        int_min_coords[j] = fmaxf(min_coords[j], box->min_coords[j]);
        int_max_coords[j] = fminf(max_coords[j], box->max_coords[j]);
        if(int_min_coords[j] > int_max_coords[j])
        {
            return NULL;
        }
    }
    return new AABB( int_min_coords[0], int_max_coords[0],
                        int_min_coords[1], int_max_coords[1],
                        int_min_coords[2], int_max_coords[2] );
}

void gcm::AABB::findIntersection(AABB* box, AABB* intersection)
{
    AABB* created = findIntersection(box);
    if( created == NULL )
        return;
    *intersection = *created;
    delete created;
}

void gcm::AABB::transfer(float x, float y, float z)
{
    min_coords[0] += x;
    min_coords[1] += y;
    min_coords[2] += z;
    max_coords[0] += x;
    max_coords[1] += y;
    max_coords[2] += z;
}

float gcm::AABB::getVolume()
{
    return (maxX - minX) * (maxY - minY) * (maxZ - minZ);
}
