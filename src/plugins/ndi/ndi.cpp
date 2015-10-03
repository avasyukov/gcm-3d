#include "ndi.hpp"

#include <vector>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include "launcher/util/helpers.hpp"

#include "SimpleVolumeSensor.hpp"

using namespace gcm;
using namespace launcher;
using xml::NodeList;
using std::get;
using std::vector;
using std::string;
using boost::lexical_cast;
namespace bfs = boost::filesystem;

NDIPlugin::NDIPlugin(Engine* engine): engine(engine)
{
    INIT_LOGGER("gcm.plugins.NDI");
}

void NDIPlugin::parseTask(xml::Doc& doc)
{
    doc.registerNamespace("ndi", "gcm3d.plugins.ndi");
    auto emitters = doc.getRootElement().xpath("/task/ndi:emitter");
    auto sensors = doc.getRootElement().xpath("/task/ndi:sensor");
    // TODO implement different emitter types
    for (auto& emitter: emitters)
    {
        string name = emitter["name"];
        NodeList areaNodes = emitter.getChildrenByName("area");
        NodeList valuesNodes = emitter.getChildrenByName("values");
        if (areaNodes.size() != 1)
            THROW_INVALID_INPUT("Exactly one area element should be provided for initial state");
        if (valuesNodes.size() != 1)
            THROW_INVALID_INPUT("Exactly one values element should be provided for initial state");

        real values[9];

        xml::Node valuesNode = valuesNodes.front();

        vector<string> names = {"vx", "vy", "vz", "sxx", "sxy", "sxz", "syy", "syz", "szz"};

        int i = 0;
        for (auto value_name: names)
        {
            string v = valuesNode.getAttributes()[value_name];
            values[i++] = v.empty() ? 0.0 : lexical_cast<real>(v);
        }

        Area* area = readArea(areaNodes.front());
        if(area == NULL)
            THROW_INVALID_INPUT("Can not read area");

        for (int i = 0; i < engine->getNumberOfBodies(); i++ )
        {
            engine->getBody(i)->setInitialState(area, values);
            engine->getBody(i)->getMeshes()->processStressState();
        }

        if (emitter.getAttributeByName("sensor", "false") == "true")
            this->sensors.push_back(new SimpleVolumeSensor(name, area, engine));
    }

    // TODO implement different sensor types
    for (auto& sensor: sensors)
    {
        string name = sensor["name"];
        NodeList areaNodes = sensor.getChildrenByName("area");
        if (areaNodes.size() != 1)
            THROW_INVALID_INPUT("Exactly one area element should be provided for initial state");

        Area* area = readArea(areaNodes.front());
        if(area == NULL)
            THROW_INVALID_INPUT("Can not read area");

        this->sensors.push_back(new SimpleVolumeSensor(name, area, engine));
    }
}

void NDIPlugin::onCalculationStepDone(){
    LOG_INFO("Saving sensors data");

    auto dir = bfs::path(engine->getOption(Engine::Options::SNAPSHOT_OUTPUT_DIRECTORY));
    for (auto sensor: sensors)
        sensor->saveData((dir/bfs::path("sensor_" + sensor->getName() + ".csv")).string());
}

NDIPlugin* gcm_plugin_create(Engine* engine)
{
    return new NDIPlugin(engine);
}