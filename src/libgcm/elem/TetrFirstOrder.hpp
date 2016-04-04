#ifndef GCM_TETR_H_
#define GCM_TETR_H_

#include <iostream>


namespace gcm {
    /*
     * Tetrahedron base class to inherit from.
     */
    class TetrFirstOrder {
    public:
        TetrFirstOrder();
        TetrFirstOrder(int n, int v[4], int mat = -1);
        /*
         * Number of current tetr
         */
        int number;
        /*
         * List of tetr vertices.
         */
        int verts[4];
        int mat = -1;
    };
}

namespace std {
    inline std::ostream& operator<< (std::ostream &os, const gcm::TetrFirstOrder &tetr) {
        os << "\n\tTetr number: " << tetr.number << "\n";
        os << "\tVerts:";
        for( int i = 0; i < 4; i++ )
            os << " " << tetr.verts[i];
        return os;
    }
}

#endif
