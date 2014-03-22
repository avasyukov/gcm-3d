#ifndef GCM_TRIANGLE_H_
#define GCM_TRIANGLE_H_

#include "elem/TetrFirstOrder.h"

namespace gcm {
    /*
     * Triangle base class to inherit from.
     */
    class TriangleFirstOrder {
    public:
        TriangleFirstOrder();
        TriangleFirstOrder(int n, int v[3]);
        /*
         * Number of current tetr
         */
        int number;
        /*
         * List of tetr vertices.
         */
        int verts[3];
    };
}
#endif
