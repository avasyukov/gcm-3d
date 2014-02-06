#include "elem/TetrFirstOrder.h"

gcm::TetrFirstOrder::TetrFirstOrder() {
	number = -1;
	for( int j = 0; j < 4; j++ )
		verts[j] = -1;
}

gcm::TetrFirstOrder::TetrFirstOrder(int n, int v[4]) {
	number = n;
	for( int j = 0; j < 4; j++ )
		verts[j] = v[j];
}
