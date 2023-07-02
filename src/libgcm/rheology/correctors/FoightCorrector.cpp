#include "libgcm/rheology/correctors/FoightCorrector.hpp"
#include "libgcm/rheology/Plasticity.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/mesh/cube/RectangularCutCubicMesh.hpp"
using namespace gcm;
using std::vector;
using std::pair;


void FoightCorrector::correctNodeState(ICalcNode& node, Mesh& mesh, const MaterialPtr& material, float time_step)
{
    if (mesh.getType() != "cube") {
        THROW_INVALID_INPUT("Bad mesh for Foight corrector");
    }

    auto props = material->getPlasticityProperties();
    real FoightEta = props[PLASTICITY_TYPE_FOIGHT][PLASTICITY_PROP_FOIGHT_ETA];
    int number = node.number;
    CalcNode& newNode = mesh.getNewNode(number);
    CalcNode& oldNode = mesh.getNode(number);

    float ax_node = (newNode.vx - oldNode.vx) / time_step;
    float ay_node = (newNode.vy - oldNode.vy) / time_step;
    float az_node = (newNode.vz - oldNode.vz) / time_step;

    vector<int> result;
    BasicCubicMesh& cubeMesh = dynamic_cast<BasicCubicMesh&>(mesh);
    cubeMesh.findNearestsNodesForNode(node.coords, result);

    if (result[0] < 0) result[0] = result[1];
    if (result[1] >= mesh.getNodesNumber()) result[1] = result[0];
    float ax_node_lx = (mesh.getNewNode(result[0]).vx - mesh.getNode(result[0]).vx) / time_step;
    float ax_node_rx = (mesh.getNewNode(result[1]).vx - mesh.getNode(result[1]).vx) / time_step;
    float ay_node_lx = (mesh.getNewNode(result[0]).vy - mesh.getNode(result[0]).vy) / time_step;
    float ay_node_rx = (mesh.getNewNode(result[1]).vy - mesh.getNode(result[1]).vy) / time_step;
    float az_node_lx = (mesh.getNewNode(result[0]).vz - mesh.getNode(result[0]).vz) / time_step;
    float az_node_rx = (mesh.getNewNode(result[1]).vz - mesh.getNode(result[1]).vz) / time_step;

    if (result[2] < 0) result[2] = result[3];
    if (result[3] >= mesh.getNodesNumber()) result[3] = result[2];
    float ax_node_ly = (mesh.getNewNode(result[2]).vx - mesh.getNode(result[2]).vx) / time_step;
    float ax_node_ry = (mesh.getNewNode(result[3]).vx - mesh.getNode(result[3]).vx) / time_step;
    float ay_node_ly = (mesh.getNewNode(result[2]).vy - mesh.getNode(result[2]).vy) / time_step;
    float ay_node_ry = (mesh.getNewNode(result[3]).vy - mesh.getNode(result[3]).vy) / time_step;
    float az_node_ly = (mesh.getNewNode(result[2]).vz - mesh.getNode(result[2]).vz) / time_step;
    float az_node_ry = (mesh.getNewNode(result[3]).vz - mesh.getNode(result[3]).vz) / time_step;

    if (result[4] < 0) result[4] = result[5];
    if (result[5] >= mesh.getNodesNumber()) result[5] = result[4];
    float ax_node_lz = (mesh.getNewNode(result[4]).vx - mesh.getNode(result[4]).vx) / time_step;
    float ax_node_rz = (mesh.getNewNode(result[5]).vx - mesh.getNode(result[5]).vx) / time_step;
    float ay_node_lz = (mesh.getNewNode(result[4]).vy - mesh.getNode(result[4]).vy) / time_step;
    float ay_node_rz = (mesh.getNewNode(result[5]).vy - mesh.getNode(result[5]).vy) / time_step;
    float az_node_lz = (mesh.getNewNode(result[4]).vz - mesh.getNode(result[4]).vz) / time_step;
    float az_node_rz = (mesh.getNewNode(result[5]).vz - mesh.getNode(result[5]).vz) / time_step;

    float a11 = (ax_node_lx - 2 * ax_node + ax_node_rx) / (cubeMesh.getHx() * cubeMesh.getHx());
    float a12 = (ay_node_lx - 2 * ay_node + ay_node_rx) / (cubeMesh.getHy() * cubeMesh.getHy());
    float a13 = (az_node_lx - 2 * az_node + az_node_rx) / (cubeMesh.getHz() * cubeMesh.getHz());

    float a21 = (ax_node_ly - 2 * ax_node + ax_node_ry) / (cubeMesh.getHx() * cubeMesh.getHx());
    float a22 = (ay_node_ly - 2 * ay_node + ay_node_ry) / (cubeMesh.getHy() * cubeMesh.getHy());
    float a23 = (az_node_ly - 2 * az_node + az_node_ry) / (cubeMesh.getHz() * cubeMesh.getHz());

    float a31 = (ax_node_lz - 2 * ax_node + ax_node_rz) / (cubeMesh.getHx()) * cubeMesh.getHx();
    float a32 = (ay_node_lz - 2 * ay_node + ay_node_rz) / (cubeMesh.getHy() * cubeMesh.getHy());
    float a33 = (az_node_lz - 2 * az_node + az_node_rz) / (cubeMesh.getHz() * cubeMesh.getHz());


    // Diagonal
    node.stress[0] +=  FoightEta * time_step * a11;
    node.stress[3] +=  FoightEta * time_step * a22;
    node.stress[5] +=  FoightEta * time_step * a33;

    // Non-diagonal
    node.stress[1] +=  FoightEta * time_step * 1/2 * (a12 + a21);
    node.stress[2] +=  FoightEta * time_step * 1/2 * (a13 + a31);
    node.stress[4] +=  FoightEta * time_step * 1/2 * (a23 + a32);

    //std::cout << node.stress[5] << std::endl;
}