#include "ndi.hpp"

#include <vector>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include "launcher/util/helpers.hpp"

#include "SimpleVolumeSensor.hpp"
#include "TimeFrameSensor.hpp"
#include "libgcm/util/areas/CylinderArea.hpp"
#include "libgcm/util/forms/StepPulseForm.hpp"
#include "libgcm/util/forms/SinusGaussForm.hpp"

using namespace gcm;
using namespace launcher;
using xml::NodeList;
using std::get;
using std::vector;
using std::string;
using std::to_string;
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

    for (auto& emitter: emitters)
    {
        string name = emitter["name"];
        string type = emitter.getAttributeByName("type", "NOTYPE");
        if (type == "NOTYPE")
        {
            NodeList areaNodes = emitter.getChildrenByName("area");
            NodeList valuesNodes = emitter.getChildrenByName("values");
            if (areaNodes.size() != 1)
                THROW_INVALID_INPUT("Exactly one area element should be provided for initial state");
            if (valuesNodes.size() != 1)
                THROW_INVALID_INPUT("Exactly one values element should be provided for initial state");

            real values[9];

            xml::Node valuesNode = valuesNodes.front();

            vector <string> names = {"vx", "vy", "vz", "sxx", "sxy", "sxz", "syy", "syz", "szz"};

            int i = 0;
            for (auto value_name: names) {
                string v = valuesNode.getAttributes()[value_name];
                values[i++] = v.empty() ? 0.0 : lexical_cast<real>(v);
            }

            Area *area = readArea(areaNodes.front());
            if (area == NULL)
                THROW_INVALID_INPUT("Can not read area");

            for (int i = 0; i < engine->getNumberOfBodies(); i++) {
                engine->getBody(i)->setInitialState(area, values);
                engine->getBody(i)->getMeshes()->processStressState();
            }

            if (emitter.getAttributeByName("sensor", "false") == "true")
                this->sensors.push_back(new SimpleVolumeSensor(name, area, engine));
        }
        else if (type == "timeframe")
        {
            NodeList areaNodes = emitter.getChildrenByName("area");
            NodeList valuesNodes = emitter.getChildrenByName("values");
            if (areaNodes.size() != 1)
                THROW_INVALID_INPUT("Exactly one area element should be provided for initial state");
            if (valuesNodes.size() != 1)
                THROW_INVALID_INPUT("Exactly one values element should be provided for initial state");

            real values[9];

            xml::Node valuesNode = valuesNodes.front();

            vector <string> names = {"vx", "vy", "vz", "sxx", "sxy", "sxz", "syy", "syz", "szz"};

            int i = 0;
            for (auto value_name: names) {
                string v = valuesNode.getAttributes()[value_name];
                values[i++] = v.empty() ? 0.0 : lexical_cast<real>(v);
            }

            Area *area = readArea(areaNodes.front());
            if (area == NULL)
                THROW_INVALID_INPUT("Can not read area");

            float start = lexical_cast<real>(emitter["start"]);
            float finish = lexical_cast<real>(emitter["finish"]);

            string form = emitter.getAttributeByName("form", "step");
            float omega = lexical_cast<real>(emitter.getAttributeByName("omega", "0"));
            float tau = lexical_cast<real>(emitter.getAttributeByName("tau", "0"));
            float duration = lexical_cast<real>(emitter.getAttributeByName("duration", "0"));
            engine->getBorderCalculator("ExternalForceCalculator")->setParameters(emitter);
            PulseForm* f = form == "sinusgauss"
                           ? static_cast<PulseForm*>(new SinusGaussForm(omega, tau, 0))
                           : static_cast<PulseForm*>(new StepPulseForm(start, duration));

            unsigned int conditionId = engine->addBorderCondition(
                    new BorderCondition(NULL, f,
                                        engine->getBorderCalculator("ExternalForceCalculator"))
            );

            for (int i = 0; i < engine->getNumberOfBodies(); i++) {
                engine->getBody(i)->setBorderCondition(area, conditionId);
            }

            if (emitter.getAttributeByName("sensor", "false") == "true")
                this->sensors.push_back(new TimeFrameSensor(name, area, engine, start, finish));
        }
        else if (type == "cscan")
        {
            LOG_INFO("C-scan is in progress. Please note that it only works in XY plane.");
            float min_x = lexical_cast<real>(emitter["min_x"]);
            float min_y = lexical_cast<real>(emitter["min_y"]);
            float step_x = lexical_cast<real>(emitter["step_x"]);
            float step_y = lexical_cast<real>(emitter["step_y"]);
            unsigned int n_x = lexical_cast<unsigned int>(emitter["n_x"]);
            unsigned int n_y = lexical_cast<unsigned int>(emitter["n_y"]);
            float r = lexical_cast<real>(emitter["r"]);
            float dt  = lexical_cast<real>(emitter["dt"]);
            float z  = lexical_cast<real>(emitter["z"]);
            string form = emitter.getAttributeByName("form", "step");

            float omega = lexical_cast<real>(emitter.getAttributeByName("omega", "0"));
            float tau = lexical_cast<real>(emitter.getAttributeByName("tau", "0"));
            float duration = lexical_cast<real>(emitter.getAttributeByName("duration", "0"));

            for (unsigned int i = 0; i < n_x; ++i)
                for (unsigned int j = 0; j < n_y; ++j)
                {
                    engine->getBorderCalculator("ExternalForceCalculator")->setParameters(emitter);
                    auto area = new CylinderArea(r, min_x + step_x * i, min_y + step_y * j, z - 0.1,
                                                 min_x + step_x * i, min_y + step_y * j, z + 0.1);

                    PulseForm* f = form == "sinusgauss"
                            ? static_cast<PulseForm*>(new SinusGaussForm(omega, tau, dt * (j + i * n_y)))
                            : static_cast<PulseForm*>(new StepPulseForm(dt * (j + i * n_y), duration));

                    unsigned int conditionId = engine->addBorderCondition(
                            new BorderCondition(NULL, f,
                                                engine->getBorderCalculator("ExternalForceCalculator"))
                    );

                    for (int i = 0; i < engine->getNumberOfBodies(); i++) {
                        engine->getBody(i)->setBorderCondition(area, conditionId);
                    }
                }

            if (emitter.getAttributeByName("reflection", "false") == "true")
                for (unsigned int i = 0; i < n_x; ++i)
                    for (unsigned int j = 0; j < n_y; ++j)
                    {
                        float zr  = lexical_cast<real>(emitter["z_reflection"]);
                        string sname = name + "_r_" + to_string(i) + to_string(j);
                        auto area = new CylinderArea(r, min_x + step_x * i, min_y + step_y * j, zr - 0.1,
                                                     min_x + step_x * i, min_y + step_y * j, zr + 0.1);
                        this->sensors.push_back(new SimpleVolumeSensor(sname, area, engine));
                    }

            if (emitter.getAttributeByName("transmission", "false") == "true")
                for (unsigned int i = 0; i < n_x; ++i)
                    for (unsigned int j = 0; j < n_y; ++j)
                    {
                        float zt  = lexical_cast<real>(emitter["z_transmission"]);
                        string sname = name + "_t_" + to_string(i) + to_string(j);
                        auto area = new CylinderArea(r, min_x + step_x * i, min_y + step_y * j, zt - 0.1,
                                                     min_x + step_x * i, min_y + step_y * j, zt + 0.1);
                        this->sensors.push_back(new SimpleVolumeSensor(sname, area, engine));
                    }
        }
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
