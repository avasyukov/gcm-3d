#include "libgcm/rheology/setters/LinearPlasticityRheologyMatrixSetter.hpp"
#include "libgcm/Exception.hpp"
#include "libgcm/rheology/Plasticity.hpp"

using namespace gcm;

void LinearPlasticityRheologyMatrixSetter::getPlasticityProps(MaterialPtr material, float& yieldLimit, float& normE)
{
    auto props = material->getPlasticityProperties();
    yieldLimit = props[PLASTICITY_TYPE_LINEAR][PLASTICITY_PROP_YIELD_LIMIT];
    normE = props[PLASTICITY_TYPE_LINEAR][PLASTICITY_PROP_NORM_E];
}

unsigned int LinearPlasticityRheologyMatrixSetter::getNumberOfStates() const
{
    return 0;
}

unsigned int LinearPlasticityRheologyMatrixSetter::getStateForNode(const ICalcNode& node) const
{
    return 0;
}

void LinearPlasticityRheologyMatrixSetter::setX(gcm_matrix& a, const MaterialPtr& material)
{
    float yieldLimit, normE;
    getPlasticityProps(material, yieldLimit, normE);

    THROW_UNSUPPORTED("Not implemented");
}

void LinearPlasticityRheologyMatrixSetter::setY(gcm_matrix& a, const MaterialPtr& material)
{
    float yieldLimit, normE;
    getPlasticityProps(material, yieldLimit, normE);

    THROW_UNSUPPORTED("Not implemented");
}

void LinearPlasticityRheologyMatrixSetter::setZ(gcm_matrix& a, const MaterialPtr& material)
{
    float yieldLimit, normE;
    getPlasticityProps(material, yieldLimit, normE);

    THROW_UNSUPPORTED("Not implemented");
}
