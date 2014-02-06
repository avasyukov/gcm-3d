#ifndef _GCM_PULSE_FORM_STEP_H
#define _GCM_PULSE_FORM_STEP_H 1

#include "util/forms/PulseForm.h"

class StepPulseForm : public PulseForm
{
public:
	StepPulseForm(float _startTime, float _duration);
	float calcMagnitudeNorm( float time, float coords[3], Area* area );
};

#endif
