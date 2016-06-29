#include "libgcm/util/forms/SinusGaussForm.hpp"

using namespace gcm;

float SinusGaussForm::calcMagnitudeNorm( float time, float coords[3], Area* area )
{
    float t = time - startTime;
    return cos(omega*t)*exp(-t*t/(2*tau));
};


