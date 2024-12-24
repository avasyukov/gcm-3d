#include "libgcm/rheology/correctors/PiezoCorrector.hpp"
#include "libgcm/rheology/Plasticity.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/Engine.hpp"
#include "libgcm/mesh/Mesh.hpp"
#include "libgcm/util/forms/StepPulseForm.hpp"

using namespace gcm;

PiezoCorrector::PiezoCorrector(real fx, real fy, real fz, real start, real duration): fx(fx), fy(fy), fz(fz), form(new StepPulseForm(start, duration)) {};

void PiezoCorrector::correctNodeState(ICalcNode& node, Mesh& mesh, const MaterialPtr& material, float time_step)
{
    auto props = material->getPlasticityProperties();
    real s = props[PLASTICITY_TYPE_PIEZO][PLASTICITY_PROP_PIEZO_S];
    real factor = form->calcMagnitudeNorm(Engine::getInstance().getCurrentTime(), node.coords, nullptr);
    node.stress[0] += s * fx * factor;
    node.stress[3] += s * fy * factor;
    node.stress[5] += s * fz * factor;
}
