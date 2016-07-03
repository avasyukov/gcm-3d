#include "ndi.hpp"

#include <vector>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include "launcher/util/helpers.hpp"

#include "SimpleVolumeSensor.hpp"
#include "libgcm/util/areas/BoxArea.hpp"
#include "libgcm/util/forms/SinusGaussForm.hpp"
#include "libgcm/util/forms/StepPulseForm.hpp"



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
        if (areaNodes.size() != 1)
            THROW_INVALID_INPUT("Exactly one area element should be provided for initial state");

        Area* area = readArea(areaNodes.front());
        if(area == NULL)
            THROW_INVALID_INPUT("Can not read area");

        if (emitter.getAttributeByName("type", "false") == "false")     //basic emitter
        {
            NodeList valuesNodes = emitter.getChildrenByName("values");
            real values[9];

            xml::Node valuesNode = valuesNodes.front();

            vector<string> names = {"vx", "vy", "vz", "sxx", "sxy", "sxz", "syy", "syz", "szz"};

            int i = 0;
            for (auto value_name: names)
            {
                string v = valuesNode.getAttributes()[value_name];
                values[i++] = v.empty() ? 0.0 : lexical_cast<real>(v);
            }
            if (valuesNodes.size() != 1)
                THROW_INVALID_INPUT("Exactly one values element should be provided for initial state");
            for (int i = 0; i < engine->getNumberOfBodies(); i++ )
            {
                engine->getBody(i)->setInitialState(area, values);
                engine->getBody(i)->getMeshes()->processStressState();
            }
            if (emitter.getAttributeByName("sensor", "false") == "true")
                this->sensors.push_back(new SimpleVolumeSensor(name, area, engine));
        }

        else if (emitter.getAttributeByName("type", "false") == "array") //phased arrat
        {       

            //building sensors
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
            if (emitter.getAttributeByName("sensor", "false") == "true")
            {
                for (int i=0; i<n; i++)
                {
                    if (a == "x")
                        nb = new BoxArea(min + i*w, min + (i+1)*w, b->minY, b->maxY, b->minZ, b->maxZ);
                    else if (a == "y")
                        nb = new BoxArea(b->minX, b->maxX, min + i*w, min + (i+1)*w, b->minZ, b->maxZ);
                    else
                        nb = new BoxArea(b->minX, b->maxX, b->minY, b->maxY, min + i*w, min + (i+1)*w);
                    this->sensors.push_back(new SimpleVolumeSensor(name + lexical_cast<string>(i), nb, engine)); // <--actually building sensors
                }
            }


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
                dt = - (x[i] - Xmax) / vel;                
                LOG_INFO("dt " <<dt);
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

        if (sensor.getAttributeByName("type", "false") == "false")
            this->sensors.push_back(new SimpleVolumeSensor(name, area, engine));
        else if (sensor.getAttributeByName("type", "false") == "array")
        {
            float min, max;
            int n = lexical_cast<int>(sensor.getAttributeByName("n"));
            string a = sensor.getAttributeByName("axis");
            BoxArea* b = dynamic_cast<BoxArea*>(area), *nb;
            if (!b) THROW_INVALID_INPUT("Wrong type of area");
            if (a == "x")
                { min = b->minX; max = b->maxX; }
            else if (a == "y")
                { min = b->minY; max = b->maxY; }
            else if (a == "z")
                { min = b->minZ; max = b->maxZ; }
            float w = (max - min) / n;
            for (int i=0; i<n; i++)
            {
                if (a == "x")
                    nb = new BoxArea(min + i*w, min + (i+1)*w, b->minY, b->maxY, b->minZ, b->maxZ);
                else if (a == "y")
                    nb = new BoxArea(b->minX, b->maxX, min + i*w, min + (i+1)*w, b->minZ, b->maxZ);
                else
                    nb = new BoxArea(b->minX, b->maxX, b->minY, b->maxY, min + i*w, min + (i+1)*w);
                this->sensors.push_back(new SimpleVolumeSensor(name + lexical_cast<string>(i), nb, engine));
            }
        }
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
