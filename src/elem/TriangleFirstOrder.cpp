/* 
 * File:   TriangleFirstOrder.cpp
 * Author: anganar
 * 
 * Created on May 3, 2013, 11:07 AM
 */

#include "TriangleFirstOrder.h"

gcm::TriangleFirstOrder::TriangleFirstOrder() {
	number = -1;
	for( int i = 0; i < 3; i++ )
		verts[i] = -1;
}

gcm::TriangleFirstOrder::TriangleFirstOrder(int n, int v[3])
{
	number = n;
	for( int i = 0; i < 3; i++ )
		verts[i] = v[i];
}
