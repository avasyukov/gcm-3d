#include "libgcm/util/forms/LinearPulseForm.hpp"

using namespace gcm;

float LinearPulseForm::calcMagnitudeNorm( float time, float coords[3], Area* area )
{
    if( time < startTime )
        return 0.0;
    if( time <= startTime + duration )
        return (time - startTime) / duration;
    return 0.0;
};


