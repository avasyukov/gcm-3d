#ifndef GCM_3D_NDI_PLUGIN_HPP
#define GCM_3D_NDI_PLUGIN_HPP

#include "libgcm/Engine.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/Plugin.hpp"
#include "launcher/util/xml.hpp"

#include "Sensor.hpp"

#include <vector>

class NDIPlugin: public gcm::Plugin
{
protected:
    gcm::Engine* engine = nullptr;
    std::vector<Sensor*> sensors;
    USE_LOGGER;
public:
    NDIPlugin(gcm::Engine* engine);
    void parseTask(xml::Doc& doc) override;
    void onCalculationStepDone() override;
};

extern "C" NDIPlugin* gcm_plugin_create(gcm::Engine* engine);

#endif //GCM_3D_NDI_HPP