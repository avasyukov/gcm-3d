#include "libgcm/mesh/markers/MarkeredBoxMeshGenerator.hpp"

#include "libgcm/node/CalcNode.hpp"

const string gcm::MarkeredBoxMeshGenerator::PARAM_MESH_EDGE = "meshEdge";
const string gcm::MarkeredBoxMeshGenerator::PARAM_CUBE_EDGE = "cubeEdge";
const string gcm::MarkeredBoxMeshGenerator::PARAM_CELL_NUM = "pointNum";

string gcm::MarkeredBoxMeshGenerator::getType(){
    return "markered-box";
}

gcm::MarkeredBoxMeshGenerator::MarkeredBoxMeshGenerator() {
    INIT_LOGGER("gcm.MarkeredBoxMeshLoader");
}

void gcm::MarkeredBoxMeshGenerator::loadMesh(Params params, MarkeredMesh* mesh,
        GCMDispatcher* dispatcher) {
    // get mesh parameters
    float mesh_edge = atof(params[PARAM_MESH_EDGE].c_str());
    float cube_edge = atof(params[PARAM_CUBE_EDGE].c_str());
    int cells_num =atoi(params[PARAM_CELL_NUM].c_str());

    float points_num = 1 + cells_num;
    float h = mesh_edge / cells_num;

    mesh->setNumberOfCells(cells_num, cells_num, cells_num);

    // generate mesh
    LOG_DEBUG("Generating nodes");
    for (int k = 0; k < points_num; k++)
        for (int j = 0; j < points_num; j++)
            for (int i = 0; i < points_num; i++) {
                int n = mesh->getPointNumber(i, j, k);
                CalcNode node;
                node.x = -mesh_edge/2 + i*h;
                node.y = -mesh_edge/2 + j*h;
                node.z = -mesh_edge/2 + k*h;
                node.number = n;
                node.setPlacement(true);
                mesh->addNode(node);
            }

    // set cell inner flags
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
    minX = minY = minZ = -cube_edge/2;
    maxX = maxY = maxZ = cube_edge/2;

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

void gcm::MarkeredBoxMeshGenerator::preLoadMesh(Params params, AABB* scene,
        int& sliceDirection, int& numberOfNodes) {
}
