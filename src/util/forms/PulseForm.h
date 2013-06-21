#ifndef _GCM_PULSE_FORM_H
#define _GCM_PULSE_FORM_H 1

#include "../../node/ElasticNode.h"
#include "../../Exception.h"
#include "../areas/Area.h"

class PulseForm
{
public:
	PulseForm(float _startTime, float _duration);
	~PulseForm();
	virtual float calcMagnitudeNorm( float time, float coords[3], Area* area ) = 0;
	bool isActive(float time);
protected:
	float startTime;
	float duration;
};

#endif
