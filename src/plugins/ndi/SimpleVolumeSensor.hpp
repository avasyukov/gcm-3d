#ifndef GCM_3D_NDI_PLUGIN_SIMPLEVOLUMESENSOR_HPP
#define GCM_3D_NDI_PLUGIN_SIMPLEVOLUMESENSOR_HPP

#include "Sensor.hpp"

class SimpleVolumeSensor: public Sensor {
public:
    SimpleVolumeSensor(const std::string& name, const gcm::Area* area, gcm::Engine* engine);

    void saveData(const std::string& fileName) override;
};


#endif //GCM_3D_SIMPLEVOLUMESENSOR_HPP
