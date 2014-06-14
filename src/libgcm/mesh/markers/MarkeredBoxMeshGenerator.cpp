#include "libgcm/mesh/markers/MarkeredBoxMeshGenerator.hpp"

#include "libgcm/node/CalcNode.hpp"

gcm::MarkeredBoxMeshGenerator::MarkeredBoxMeshGenerator() {
    INIT_LOGGER("gcm.MarkeredBoxMeshLoader");
}

void gcm::MarkeredBoxMeshGenerator::loadMesh(MarkeredMesh* mesh, GCMDispatcher* dispatcher, float meshEdge, float cubeEdge, int cellsNum) {
    float points_num = 1 + cellsNum;
    float h = meshEdge / cellsNum;

    mesh->setNumberOfCells(cellsNum, cellsNum, cellsNum);

    // generate mesh
    LOG_DEBUG("Generating nodes");
    for (int k = 0; k < points_num; k++)
        for (int j = 0; j < points_num; j++)
            for (int i = 0; i < points_num; i++) {
                int n = mesh->getPointNumber(i, j, k);
                CalcNode node;
                node.x = -meshEdge/2 + i*h;
                node.y = -meshEdge/2 + j*h;
                node.z = -meshEdge/2 + k*h;
                node.number = n;
                node.setPlacement(true);
                mesh->addNode(node);
            }

    // set cell inner flags
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
    minX = minY = minZ = -cubeEdge/2;
    maxX = maxY = maxZ = cubeEdge/2;

    LOG_DEBUG("Setting cells inner flag");
    AABB cubeAABB(minX, maxX, minY, maxY, minZ, maxZ);

    for (unsigned int i = 0; i < mesh->getTotalNumberOfCells(); i++) {
        AABB aabb;
        mesh->getCellAABB(i, aabb);
        if (cubeAABB.includes(&aabb))
            mesh->setCellInnerFlag(i, true);
    }

    mesh->preProcess();
}


gcm::MarkeredBoxMeshGenerator::~MarkeredBoxMeshGenerator() {
}

void gcm::MarkeredBoxMeshGenerator::preLoadMesh(AABB* scene, int& sliceDirection, int& numberOfNodes, float meshEdge, float cubeEdge, int cellsNum) {
}
