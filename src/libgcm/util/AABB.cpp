#include "libgcm/util/AABB.hpp"
#include "libgcm/Math.hpp"

using namespace gcm;
using std::numeric_limits;

AABB::AABB()
{
    minX = minY = minZ = numeric_limits<float>::infinity();
    maxX = maxY = maxZ = - numeric_limits<float>::infinity();
}

AABB::AABB(float _minX, float _maxX, float _minY, float _maxY, float _minZ, float _maxZ)
{
    minX = _minX;
    minY = _minY;
    minZ = _minZ;
    maxX = _maxX;
    maxY = _maxY;
    maxZ = _maxZ;
}

bool AABB::isInAABB( float x, float y, float z ) const
{
    // FIXME do we really have to check our state all the time?
    assert_ne(minX, maxX);
    assert_ne(minY, maxY);
    assert_ne(minZ, maxZ);
    return ( x < maxX + EQUALITY_TOLERANCE
                && x > minX - EQUALITY_TOLERANCE
                && y < maxY + EQUALITY_TOLERANCE
                && y > minY - EQUALITY_TOLERANCE
                && z < maxZ + EQUALITY_TOLERANCE
                && z > minZ - EQUALITY_TOLERANCE );
};

bool AABB::isInAABB(const Node* node) const
{
    return isInAABB(node->coords.x, node->coords.y, node->coords.z);
};

bool AABB::isInAABB(const Node& node) const
{
    return isInAABB(node.coords.x, node.coords.y, node.coords.z);
};

bool AABB::isOutAABB(float x, float y, float z) const {
	assert_ne(minX, maxX);
    assert_ne(minY, maxY);
    assert_ne(minZ, maxZ);
    return ( x > maxX - EQUALITY_TOLERANCE
                || x < minX + EQUALITY_TOLERANCE
                || y > maxY - EQUALITY_TOLERANCE
                || y < minY + EQUALITY_TOLERANCE
                || z > maxZ - EQUALITY_TOLERANCE
                || z < minZ + EQUALITY_TOLERANCE );
};

bool AABB::isOutAABB(const Node& node) const {
	return isOutAABB(node.coords.x, node.coords.y, node.coords.z);
};

bool AABB::includes(const AABB* box) const
{
    return ( minX <= box->minX ) && ( minY <= box->minY ) && ( minZ <= box->minZ )
            && ( maxX >= box->maxX ) && ( maxY >= box->maxY ) && ( maxZ >= box->maxZ );
}

bool AABB::includes(const AABB& box) const
{
    return includes(&box);
}

bool AABB::intersects(AABB* box)
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

/*bool AABB::intersects(AABB box)
{
    AABB* inters = findIntersection(&box);
    if(inters == NULL)
        return false;
    delete inters;
    return true;
}*/

AABB* AABB::findIntersection(AABB* box)
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

void AABB::findIntersection(AABB* box, AABB* intersection)
{
    AABB* created = findIntersection(box);
    if( created == NULL )
        return;
    *intersection = *created;
    delete created;
}

void AABB::transfer(float x, float y, float z)
{
    min_coords[0] += x;
    min_coords[1] += y;
    min_coords[2] += z;
    max_coords[0] += x;
    max_coords[1] += y;
    max_coords[2] += z;
}

void AABB::scale(float x0, float y0, float z0, 
		float scaleX, float scaleY, float scaleZ)
{
    min_coords[0] = (min_coords[0] - x0)*scaleX + x0;
    min_coords[1] = (min_coords[1] - y0)*scaleY + y0;
    min_coords[2] = (min_coords[2] - z0)*scaleZ + z0;
    max_coords[0] = (max_coords[0] - x0)*scaleX + x0;
    max_coords[1] = (max_coords[1] - y0)*scaleY + y0;
    max_coords[2] = (max_coords[2] - z0)*scaleZ + z0;
}

float AABB::getVolume()
{
    return (maxX - minX) * (maxY - minY) * (maxZ - minZ);
}

real AABB::getDiag() const
{
    return distance(&min_coords[0], &max_coords[0]);
}

vector3r AABB::getCenter() const
{
    return vector3r((maxX+minX)/2, (maxY+minY)/2, (maxZ+minZ)/2);
}
