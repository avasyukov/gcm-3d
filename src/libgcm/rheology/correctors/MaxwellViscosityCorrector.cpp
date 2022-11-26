#include "libgcm/rheology/correctors/MaxwellViscosityCorrector.hpp"
#include "libgcm/rheology/Plasticity.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/mesh/Mesh.hpp"

using namespace gcm;

void MaxwellViscosityCorrector::correctNodeState(ICalcNode& node, Mesh& mesh, const MaterialPtr& material, float time_step)
{
    auto props = material->getPlasticityProperties();
    real maxwellTau = props[PLASTICITY_TYPE_MAXWELL][PLASTICITY_PROP_MAXWELL_TAU];

    real factor = time_step / maxwellTau;

    //std::cout << node.stress[5] << std::endl;

    // Diagonal
    node.stress[0] -= node.stress[0] * factor;
    node.stress[3] -= node.stress[3] * factor;
    node.stress[5] -= node.stress[5] * factor;

    // Non-diagonal
    node.stress[1] -= node.stress[1] * factor;
    node.stress[2] -= node.stress[2] * factor;
    node.stress[4] -= node.stress[4] * factor;

    //std::cout << node.stress[5] << std::endl;
}
