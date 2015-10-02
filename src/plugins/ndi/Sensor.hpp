#ifndef GCM_3D_NDI_PLUGIN_SENSOR_HPP
#define GCM_3D_NDI_PLUGIN_SENSOR_HPP

#include "libgcm/util/areas/Area.hpp"
#include <string>
#include <tuple>
#include <vector>

namespace gcm
{
    class Engine;
    class Mesh;
}

class Sensor
{
protected:
    std::string name;
    const gcm::Area* area;
    gcm::Engine* engine;

    std::vector<std::tuple<gcm::Mesh*, std::vector<int>>> nodes;

    USE_LOGGER;
public:
    Sensor(const std::string& name, const gcm::Area* area, gcm::Engine* engine);
    virtual ~Sensor() = 0;

    virtual void saveData(const std::string& fileName) = 0;

    const std::string& getName() const;
};

#endif //GCM_3D_SENSOR_HPP_HPP
