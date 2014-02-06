#ifndef _GCM_SIMPLE_VOLUME_CALCULATOR_H
#define _GCM_SIMPLE_VOLUME_CALCULATOR_H  1

#include "calc/volume/VolumeCalculator.h"
#include "Logging.h"

class SimpleVolumeCalculator : public VolumeCalculator
{
public:
	SimpleVolumeCalculator();
	~SimpleVolumeCalculator();
	void do_calc(CalcNode* new_node, ElasticMatrix3D* matrix, float* values[]);
	inline string getType() {
		return "SimpleVolumeCalculator";
	}

protected:
	USE_LOGGER;

};

#endif
