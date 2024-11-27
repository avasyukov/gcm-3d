#include "libgcm/util/forms/AntiStepPulseForm.hpp"

using namespace gcm;

float AntiStepPulseForm::calcMagnitudeNorm( float time, float coords[3], Area* area )
{
    if( time < startTime )
        return 0.0;
    if( duration < 0 )
        return -1.0;
    if( time <= startTime + duration )
        return -1.0;
    return 0.0;
};


