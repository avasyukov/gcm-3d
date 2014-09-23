#include "libgcm/mesh/cube/BasicCubicMeshGenerator.hpp"

#include "libgcm/node/CalcNode.hpp"

using namespace gcm;

BasicCubicMeshGenerator::BasicCubicMeshGenerator() {
    INIT_LOGGER("gcm.BasicCubicMeshGenerator");
}

BasicCubicMeshGenerator::~BasicCubicMeshGenerator() {
}

void BasicCubicMeshGenerator::loadMesh(BasicCubicMesh* mesh, GCMDispatcher* dispatcher, float h, int num)
{
    for( int k = 0; k <= num; k++ )
        for( int j = 0; j <= num; j++ )
            for( int i = 0; i <= num; i++ )
            {
                int n = i*(num+1)*(num+1) + j*(num+1) + k;
                float x = i*h;
                float y = j*h;
                float z = k*h;
                CalcNode* node = new CalcNode();//(n, x, y, z);
                node->number = n;
                node->coords[0] = x;
                node->coords[1] = y;
                node->coords[2] = z;
                node->setPlacement(true);
                mesh->addNode( *node );
            }
    mesh->preProcess();
}

void BasicCubicMeshGenerator::preLoadMesh(AABB* scene, int& sliceDirection, int& numberOfNodes, float h, int num)
{
    sliceDirection = 0;
    numberOfNodes = (num + 1) * (num + 1) * (num + 1);
    scene->minX = scene->minY = scene->minZ = 0;
    scene->maxX = scene->maxY = scene->maxZ = num * h;
}
