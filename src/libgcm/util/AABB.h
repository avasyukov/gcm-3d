#ifndef _GCM_AABB_OUTLINE_H
#define _GCM_AABB_OUTLINE_H  1

#include <assert.h>

#include "node/Node.h"
#include "Math.h"

namespace gcm {
    class AABB {
    public:
        union {
            float min_coords[3];
            struct {
                float minX;
                float minY;
                float minZ;
            };
        };
        union {
            float max_coords[3];
            struct {
                float maxX;
                float maxY;
                float maxZ;
            };
        };
        AABB();
        AABB( float _minX, float _maxX, float _minY, float _maxY, float _minZ, float _maxZ );
        bool isInAABB( float x, float y, float z );
        bool isInAABB( Node* node );
        bool isInAABB( Node& node );
        bool includes( AABB* box );
        //bool intersects( AABB box );
        bool intersects( AABB* box );
        AABB* findIntersection( AABB* box );
        void findIntersection( AABB* box, AABB* intersection );
        void transfer( float x, float y, float z );
        float getVolume();
    };
}

namespace std {
    inline std::ostream& operator<< (std::ostream &os, const gcm::AABB &aabb) {
        os << "[" << aabb.minX << ", " << aabb.maxX << "] ";
        os << "[" << aabb.minY << ", " << aabb.maxY << "] ";
        os << "[" << aabb.minZ << ", " << aabb.maxZ << "] ";
        return os;
    }
}

#endif
