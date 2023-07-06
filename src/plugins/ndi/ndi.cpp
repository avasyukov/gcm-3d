#include "ndi.hpp"

#include <vector>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include "launcher/util/helpers.hpp"

#include "SimpleVolumeSensor.hpp"
#include "TimeFrameSensor.hpp"
#include "DelayedSensor.hpp"

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
        else if (type == "array")
        {
            NodeList areaNodes = emitter.getChildrenByName("area");
            Area *area = readArea(areaNodes.front());
            if (area == NULL)
                THROW_INVALID_INPUT("Can not read area");

            //calculating area parameters
            float min, max;
            int n = lexical_cast<int>(emitter.getAttributeByName("n"));
            string a = emitter.getAttributeByName("axis");
            BoxArea* b = dynamic_cast<BoxArea*>(area), *nb;
            if (!b) THROW_INVALID_INPUT("Wrong type of area");
            if (a == "x")
                { min = b->minX; max = b->maxX; }
            else if (a == "y")
                { min = b->minY; max = b->maxY; }
            else if (a == "z")
                { min = b->minZ; max = b->maxZ; }
            float w = (max - min) / n;

            //calculator initialization - one for all coditions
            xml::Node borderConditionNode = emitter.getChildrenByName("borderCondition").front();
            string calculator = borderConditionNode["calculator"];
            if( engine->getBorderCalculator(calculator) == NULL )
                THROW_INVALID_INPUT("Unknown border calculator requested: " + calculator);
            engine->getBorderCalculator(calculator)->setParameters( borderConditionNode );
            string calcType = lexical_cast<string>(borderConditionNode.getAttributeByName("type", ""));

            //reading parameters
            float omega = lexical_cast<real>(borderConditionNode.getAttributeByName("omega", "0"));
            float tau = lexical_cast<real>(borderConditionNode.getAttributeByName("tau", "0"));
            float startTime = lexical_cast<real>(borderConditionNode.getAttributeByName("startTime", "0"));
            float duration = lexical_cast<real>(borderConditionNode.getAttributeByName("duration", "0"));
            float F = lexical_cast<real>(emitter.getAttributeByName("F", "0"));
            float dF = lexical_cast<real>(emitter.getAttributeByName("dF", "0"));

            //calculating the focusing - delay parameters for each of n emitters
            float M = (max - min)/2, m, dt;
            vector<float> x;
            float R = sqrt(F*F + dF*dF), Xmax;
            float vel = engine->getBody(0)->getMeshes()->getMaxEigenvalue();//getAvgH() / engine->getTimeStep();
            for (int i=0; i<n; i++)
            {
                //calculate phase
                m = M - 2*i*M/n;
                x.push_back(sqrt((m+dF)*(m+dF) + F*F) - F);
                if (!i) Xmax = x[i];
                if (x[i] > Xmax) Xmax = x[i];
            }

            //creating conditions
            unsigned int conditionId = -1;
            for (int i=0; i<n; i++)
            {
                dt = (Xmax - x[i]) / vel;
                if (F == 0) dt = 0;
                //LOG_INFO("dt " <<dt);
                if (calcType == "sinus_gauss")
                    conditionId = engine->addBorderCondition(
                        new BorderCondition(NULL, new SinusGaussForm(omega, tau, dt), engine->getBorderCalculator(calculator) )
                    );
                else if (calcType == "step")
                    conditionId = engine->addBorderCondition(
                        new BorderCondition(NULL, new StepPulseForm(startTime, duration), engine->getBorderCalculator(calculator) )
                    );
                LOG_INFO("Border condition (SinusGauss) created with calculator: " + calculator);
                BoxArea* conditionArea = NULL;
                if (a == "x")
                    conditionArea = new BoxArea(min + i*w, min + (i+1)*w, b->minY, b->maxY, b->minZ, b->maxZ);
                else if (a == "y")
                    conditionArea = new BoxArea(b->minX, b->maxX, min + i*w, min + (i+1)*w, b->minZ, b->maxZ);
                else
                    conditionArea = new BoxArea(b->minX, b->maxX, b->minY, b->maxY, min + i*w, min + (i+1)*w);
                for( int j = 0; j < engine->getNumberOfBodies(); j++ )
                {
                    engine->getBody(j)->setBorderCondition(conditionArea, conditionId);
                }
                delete conditionArea;
            }
            //building sensors
            if (emitter.getAttributeByName("sensor", "false") == "true")
            {
                for (int i=0; i<n; i++)
                {
                    dt = (x[i]) / vel;
                    if (F == 0) dt = 0;
                    //dt += tau * 35000; //magic constant tu cut the initial signal from sensors data
                    if (a == "x")
                        nb = new BoxArea(min + i*w, min + (i+1)*w, b->minY, b->maxY, b->minZ, b->maxZ);
                    else if (a == "y")
                        nb = new BoxArea(b->minX, b->maxX, min + i*w, min + (i+1)*w, b->minZ, b->maxZ);
                    else
                        nb = new BoxArea(b->minX, b->maxX, b->minY, b->maxY, min + i*w, min + (i+1)*w);
                    LOG_INFO("ss " <<i <<" n " <<name + lexical_cast<string>(i) <<" mm " <<min + i*w <<" " <<min + (i+1)*w);
                    this->sensors.push_back(new DelayedSensor(name + lexical_cast<string>(i), nb, engine, dt)); // <--actually building sensors
                }
            }
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
