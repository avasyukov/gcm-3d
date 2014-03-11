#include "rheology/StdRheologyCalculator.h"

#include <cmath>

#include "node/CalcNode.h"
#include "materials/IsotropicElasticMaterial.h"

gcm::StdRheologyCalculator::StdRheologyCalculator()
{
    INIT_LOGGER("gcm.StdRheologyCalculator");
}

void gcm::StdRheologyCalculator::doCalc(CalcNode& src, CalcNode& dst)
{

    // FIXME get rid of hardcoded elastic material
    IsotropicElasticMaterial* mat = dynamic_cast<IsotropicElasticMaterial*>(src.getMaterial());
    assert(mat);

    float K = mat->getLambda() + 2 * mat->getMu() / 3;
    dst.setRho(mat->getRho() * expf(src.getPressure() / K));
}
