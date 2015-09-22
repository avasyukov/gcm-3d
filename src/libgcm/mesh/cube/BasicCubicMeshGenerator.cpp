#include "libgcm/mesh/cube/BasicCubicMeshGenerator.hpp"

#include "libgcm/node/CalcNode.hpp"

using namespace gcm;

BasicCubicMeshGenerator::BasicCubicMeshGenerator() {
    INIT_LOGGER("gcm.BasicCubicMeshGenerator");
}

BasicCubicMeshGenerator::~BasicCubicMeshGenerator() {
}

void BasicCubicMeshGenerator::loadMesh(BasicCubicMesh* mesh, 
	GCMDispatcher* dispatcher, float h, int numX, int numY, int numZ)
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
                float x = float(i) * h;
                float y = float(j) * h;
                float z = float(k) * h;
                CalcNode* node = new CalcNode();//(n, x, y, z);
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

void BasicCubicMeshGenerator::preLoadMesh(AABB* scene, int& sliceDirection, 
	int& numberOfNodes, float h, int numX, int numY, int numZ)
{
    sliceDirection = 0;
    numberOfNodes = (numX + 1) * (numY + 1) * (numZ + 1);
    scene->minX = scene->minY = scene->minZ = 0;
    scene->maxX = float(numX) * h;
	scene->maxY = float(numY) * h;
	scene->maxZ = float(numZ) * h;
}
