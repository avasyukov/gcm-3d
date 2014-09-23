#ifndef GCM_TRIANGLE_H_
#define GCM_TRIANGLE_H_

#include "libgcm/elem/TetrFirstOrder.hpp"

namespace gcm {
    /*
     * Triangle base class to inherit from.
     */
    class TriangleFirstOrder {
    public:
        TriangleFirstOrder();
        TriangleFirstOrder(int n, int v[3]);
        TriangleFirstOrder(int n, int v1, int v2, int v3);
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
