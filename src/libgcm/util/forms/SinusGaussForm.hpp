#ifndef _GCM_SINUS_GAUSS_STEP_H
#define _GCM_SINUS_GAUSS_STEP_H 1

#include "libgcm/util/forms/PulseForm.hpp"
namespace gcm
{
    class SinusGaussForm : public PulseForm
    {
    public:
        SinusGaussForm(float _startTime, float _duration): PulseForm(_startTime, _duration) {};
        SinusGaussForm(float _o, float _t, float _s): PulseForm(_s, 0) {omega = _o, tau = _t;};
        float calcMagnitudeNorm( float time, float coords[3], Area* area );
    private:
        float omega;    //basic frequency
        float tau;      //Gaussian width
    };
}
#endif
