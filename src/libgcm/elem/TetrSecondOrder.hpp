#ifndef GCM_TETR_SECOND_ORDER_H_
#define GCM_TETR_SECOND_ORDER_H_

#include "libgcm/elem/TetrFirstOrder.hpp"

namespace gcm {
    /*
     * Tetrahedron base class to inherit from.
     */
    class TetrSecondOrder : public TetrFirstOrder {
    public:
        TetrSecondOrder();
        TetrSecondOrder(int n, int v[4]);
        TetrSecondOrder(int n, int v[4], int av[6]);
        /*
         * List of additional vertices on edges
         */
        int addVerts[6];
    };
}

namespace std {
    inline std::ostream& operator<< (std::ostream &os, const gcm::TetrSecondOrder &tetr) {
        os << "\n\tTetr number: " << tetr.number << "\n";
        os << "\tVerts:";
        for( int i = 0; i < 4; i++ )
            os << " " << tetr.verts[i];
        os << "\n\tAdditional verts:";
        for( int i = 0; i < 6; i++ )
            os << " " << tetr.addVerts[i];
        return os;
    }
}

#endif
