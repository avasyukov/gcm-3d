#include "libgcm/mesh/cube/RectangularMeshGenerator.hpp"

#include "libgcm/node/CalcNode.hpp"

using namespace gcm;

RectangularMeshGenerator::RectangularMeshGenerator() {
    INIT_LOGGER("gcm.RectangularMeshGenerator");
}

RectangularMeshGenerator::~RectangularMeshGenerator() {
}

void RectangularMeshGenerator::loadMesh(RectangularMesh* mesh, GCMDispatcher* dispatcher,
                                          int numX, int numY, int numZ, double& sx, double& sy, double& sz) 
{
	mesh->setNumX(numX);
	mesh->setNumY(numY);
	mesh->setNumZ(numZ);
	static int startNumber = 0;
    for( int k = 0; k <= numZ; k++ )
        for( int j = 0; j <= numY; j++ )
            for( int i = 0; i <= numX; i++ )
            {
				int n = i*(numY+1)*(numZ+1) + j*(numZ+1) + k + startNumber;
                float x = float(i) * sx;
                float y = float(j) * sy;
                float z = float(k) * sz;
                CalcNode* node = new CalcNode(); //(n, x, y, z);
                node->number = n;
                node->coords[0] = x;
                node->coords[1] = y;
                node->coords[2] = z;
                node->setPlacement(true);
                mesh->addNode( *node );
            }
    mesh->preProcess();
	startNumber += 100000000;
}

