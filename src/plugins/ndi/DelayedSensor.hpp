#ifndef GCM_3D_NDI_PLUGIN_DELAYEDSENSOR_HPP
#define GCM_3D_NDI_PLUGIN_DELAYEDSENSOR_HPP

#include "Sensor.hpp"

class DelayedSensor: public Sensor {
public:
    DelayedSensor(const std::string& name, const gcm::Area* area, gcm::Engine* engine, float _t);

    void saveData(const std::string& fileName) override;
private:
    float startTime;
};


#endif //GCM_3D_SIMPLEVOLUMESENSOR_HPP

