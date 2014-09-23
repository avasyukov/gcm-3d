/*
 * File:   TriangleFirstOrder.cpp
 * Author: anganar
 *
 * Created on May 3, 2013, 11:07 AM
 */
#include "libgcm/elem/TriangleFirstOrder.hpp"

gcm::TriangleFirstOrder::TriangleFirstOrder(): TriangleFirstOrder(-1, -1, -1, -1)
{
}

gcm::TriangleFirstOrder::TriangleFirstOrder(int n, int v[3]): TriangleFirstOrder(n, v[0], v[1], v[2])
{
}

gcm::TriangleFirstOrder::TriangleFirstOrder(int n, int v1, int v2, int v3)
{
    number = n;
    verts[0] = v1;
    verts[1] = v2;
    verts[2] = v3;
}
