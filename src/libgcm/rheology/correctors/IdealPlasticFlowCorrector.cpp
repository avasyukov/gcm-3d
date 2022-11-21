#include "libgcm/rheology/correctors/IdealPlasticFlowCorrector.hpp"
#include "libgcm/rheology/Plasticity.hpp"

using namespace gcm;

void IdealPlasticFlowCorrector::correctNodeState(ICalcNode& node, Mesh& mesh, const MaterialPtr& material, float time_step)
{
    auto props = material->getPlasticityProperties();
    real yieldStrength = props[PLASTICITY_TYPE_PRANDTL_RAUSS_CORRECTOR][PLASTICITY_PROP_YIELD_STRENGTH];
    
    // Hydrostatic stress
	real p = (node.stress[0] + node.stress[3] + node.stress[5]) / 3;
	
	// Second stress invariant
	real J2 = sqrt((node.stress[0] - node.stress[3])*(node.stress[0] - node.stress[3]) +
					(node.stress[3] - node.stress[5])*(node.stress[3] - node.stress[5]) +
					(node.stress[0] - node.stress[5])*(node.stress[0] - node.stress[5]) +
					6 * (node.stress[1]*node.stress[1] + node.stress[2]*node.stress[2] + node.stress[4]*node.stress[4])) / sqrt(6.0);
	
	// Correction parameter
	real x = yieldStrength / J2;
	
	if(x < 1.0) {
	// Correct stress components:
		// Diagonal
		node.stress[0] = p + x * (node.stress[0] - p);
		node.stress[3] = p + x * (node.stress[3] - p);
		node.stress[5] = p + x * (node.stress[5] - p);
		
		// Non-diagonal
		node.stress[1] = x * node.stress[1];
		node.stress[2] = x * node.stress[2];
		node.stress[4] = x * node.stress[4];
	}
}
