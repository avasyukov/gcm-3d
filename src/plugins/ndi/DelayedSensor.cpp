#include "DelayedSensor.hpp"

#include <fstream>

#include "libgcm/node/CalcNode.hpp"

using namespace gcm;
using std::string;
using std::ofstream;
using std::ios;
using std::endl;
using std::get;

DelayedSensor::DelayedSensor(const std::string& name, const Area* area, gcm::Engine* engine, float _t): Sensor(name, area, engine)
{
    INIT_LOGGER("gcm.plugins.ndi.SimpleVolumeSensor");
    if (nodes.size() != 1)
        THROW_UNSUPPORTED("SimpleVolumeSensor supports only one mesh");

    if (get<1>(nodes.front()).size() == 0)
        THROW_UNSUPPORTED("SimpleVolumeSensor need at least one node to gather values from");
    startTime = _t;
}

void DelayedSensor::saveData(const string& fileName)
{
    int zerofy = 0;
    if (engine->getCurrentTime() < startTime)
    {
        //LOG_INFO("et" <<engine->getCurrentTime() <<"st " <<startTime);
        //zerofy = 1;
        return;
    }
    real values[9] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    Mesh* mesh = get<0>(nodes.front());
    auto& _nodes = get<1>(nodes.front());
    for (auto i: _nodes)
    {
        CalcNode& node = mesh->getNode(i);
        for (int j = 0; j < 9; j++)
            values[j] += node.values[j];
    }

    ofstream out;
    out.open(fileName, ios::out | ios::app);
    out << engine->getCurrentTime();
    for (int j = 0; j < 9; j++)
    {
        //if (zerofy)
        //    out << " " << 0;
        //else
        out << " " << values[j]/_nodes.size();
    }
    out << endl;
    out.close();
}
