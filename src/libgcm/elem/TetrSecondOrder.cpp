#include "libgcm/elem/TetrSecondOrder.h"

gcm::TetrSecondOrder::TetrSecondOrder() {
    for( int j = 0; j < 6; j++ )
        addVerts[j] = -1;
}

gcm::TetrSecondOrder::TetrSecondOrder(int n, int v[4]) {
    number = n;
    for( int j = 0; j < 4; j++ )
        verts[j] = v[j];
    for( int j = 0; j < 6; j++ )
        addVerts[j] = -1;
}

gcm::TetrSecondOrder::TetrSecondOrder(int n, int v[4], int av[6]) {
    number = n;
    for( int j = 0; j < 4; j++ )
        verts[j] = v[j];
    for( int j = 0; j < 6; j++ )
        addVerts[j] = av[j];
}
