#ifndef _GCM_PULSE_FORM_LINEAR_H
#define _GCM_PULSE_FORM_LINEAR_H 1

#include "libgcm/util/forms/PulseForm.hpp"
namespace gcm
{
    class LinearPulseForm : public PulseForm
    {
    public:
        LinearPulseForm(float _startTime, float _duration): PulseForm(_startTime, _duration) {}
        float calcMagnitudeNorm( float time, float coords[3], Area* area );
    };
}
#endif
