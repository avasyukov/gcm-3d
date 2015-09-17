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

    int x1 = numX/2 - numX/10;
    int x2 = numX/2 + numX/10;
    int y1 = numY/2 - numY/10;
    int y2 = numY/2 + numY/10;
    int z1 = numZ/2 - numZ/10;
    int z2 = numZ/2 + numZ/10;

    for( int k = 0; k <= numZ; k++ )
        for( int j = 0; j <= numY; j++ )
            for( int i = 0; i <= numX; i++ )
            {
				int n = i*(numY+1)*(numZ+1) + j*(numZ+1) + k;
                float x = float(i) * h;
                float y = float(j) * h;
                float z = float(k) * h;
                CalcNode* node = new CalcNode();//(n, x, y, z);
                node->normal_flag = 0;
                node->setIsBorder(false);
                node->number = n;
                node->coords[0] = x;
                node->coords[1] = y;
                node->coords[2] = z;
                node->setPlacement(true);

                if (k >= z1 && k <= z2 && j >= y1 && j <= y2 && i >= x1 && i <= x2)
                {
                    node->setIsBorder(true);
                    if (k > z1 && k < z2 && j > y1 && j < y2 && i > x1 && i < x2)
                        node->setUsed(false);
                    else
                    {
                        if (i == x1)
                            node->normal_flag = 1;
                        else if (i == x2)
                            node->normal_flag = 2;
                        else if (j == y1)
                            node->normal_flag = 3;
                        else if (j == y2)
                            node->normal_flag = 4;
                        else if (k == z1)
                            node->normal_flag = 5;
                        else if (k == z2)
                            node->normal_flag = 6;
                    }
                }

                mesh->addNode( *node );
            }

    mesh->preProcess();
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
