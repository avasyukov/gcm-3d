#ifndef _GCM_AREA_H
#define _GCM_AREA_H 1

#include "libgcm/node/Node.hpp"
#include "libgcm/Exception.hpp"
#include "libgcm/Math.hpp"

namespace gcm {
    class Area
    {
    public:
        virtual ~Area() = 0;
        bool isInArea( Node* cur_node );
        virtual bool isInArea( Node& cur_node ) = 0;
    };
}

#endif
