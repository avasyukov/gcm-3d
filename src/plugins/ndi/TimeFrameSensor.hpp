#ifndef GCM_3D_NDI_PLUGIN_TIMEFRAMESENSOR_HPP
#define GCM_3D_NDI_PLUGIN_TIMEFRAMESENSOR_HPP

#include "Sensor.hpp"

class TimeFrameSensor: public Sensor {
    float start, finish;
public:
    TimeFrameSensor(const std::string& name, const gcm::Area* area, gcm::Engine* engine, float start, float finish);

    void saveData(const std::string& fileName) override;
};


#endif //GCM_3D_SIMPLEVOLUMESENSOR_HPP
