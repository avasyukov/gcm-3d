#include "libgcm/rheology/correctors/IdealPlasticFlowCorrector.hpp"
#include "libgcm/rheology/Plasticity.hpp"

using namespace gcm;

void gcm::IdealPlasticFlowCorrector::correctNodeState(ICalcNode& node, const MaterialPtr& material)
{
    auto props = material->getPlasticityProperties();
    gcm_real yieldStrength = props[PLASTICITY_TYPE_PRANDTL_RAUSS_CORRECTOR][PLASTICITY_PROP_YIELD_STRENGTH];
    // FIXME_ASAP
    // std::cout << "IdealPlasticFlowCorrector called\n";
}