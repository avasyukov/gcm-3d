#ifndef _GCM_SIMPLE_VOLUME_CALCULATOR_H
#define _GCM_SIMPLE_VOLUME_CALCULATOR_H  1

#include "libgcm/calc/volume/VolumeCalculator.hpp"
#include "libgcm/rheology/RheologyMatrix.hpp"
#include "libgcm/Logging.hpp"

class SimpleVolumeCalculator : public VolumeCalculator
{
public:
    SimpleVolumeCalculator();
    void doCalc(CalcNode& new_node, RheologyMatrixPtr matrix,
                                    vector<CalcNode>& previousNodes);
    inline string getType() {
        return "SimpleVolumeCalculator";
    }

protected:
    USE_LOGGER;

};

#endif
