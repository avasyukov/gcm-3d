#ifndef _GCM_SIMPLE_VOLUME_CALCULATOR_H
#define _GCM_SIMPLE_VOLUME_CALCULATOR_H  1

#include "calc/volume/VolumeCalculator.h"
#include "Logging.h"

class SimpleVolumeCalculator : public VolumeCalculator
{
public:
	SimpleVolumeCalculator();
	void doCalc(CalcNode& new_node, ElasticMatrix3D& matrix, 
									vector<CalcNode>& previousNodes);
	inline string getType() {
		return "SimpleVolumeCalculator";
	}

protected:
	USE_LOGGER;

};

#endif
