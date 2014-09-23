#include "libgcm/elem/TriangleSecondOrder.hpp"

using namespace gcm;

TriangleSecondOrder::TriangleSecondOrder() {
    for( int i = 0; i < 3; i++ )
        addVerts[i] = -1;
}

TriangleSecondOrder::TriangleSecondOrder(int n, int v[3])
{
    for( int i = 0; i < 3; i++ )
        verts[i] = -1;
}

TriangleSecondOrder::TriangleSecondOrder(int n, int v[3], int av[3])
{
    for( int i = 0; i < 3; i++ )
        verts[i] = av[i];
}
