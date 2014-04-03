#ifndef _GCM_PULSE_FORM_H
#define _GCM_PULSE_FORM_H 1

#include "util/areas/Area.h"
#include "Exception.h"

namespace gcm
{
    class PulseForm
    {
    public:
        PulseForm(float _startTime, float _duration);
        virtual ~PulseForm() = 0;
        virtual float calcMagnitudeNorm( float time, float coords[3], Area* area ) = 0;
        bool isActive(float time);
    protected:
        float startTime;
        float duration;
    };
}

#endif
