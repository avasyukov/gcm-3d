#include "TimeFrameSensor.hpp"

#include <fstream>

#include "libgcm/node/CalcNode.hpp"

using namespace gcm;
using std::string;
using std::ofstream;
using std::ios;
using std::endl;
using std::get;

TimeFrameSensor::TimeFrameSensor(const std::string& name, const Area* area, gcm::Engine* engine, float start, float finish): start(start), finish(finish), Sensor(name, area, engine)
{
    INIT_LOGGER("gcm.plugins.ndi.TimeFrameSensor");
//    if (nodes.size() != 1)
//        THROW_UNSUPPORTED("SimpleVolumeSensor supports only one mesh");

//    if (get<1>(nodes.front()).size() == 0)
//        THROW_UNSUPPORTED("SimpleVolumeSensor need at least one node to gather values from");
}

void TimeFrameSensor::saveData(const string& fileName)
{
    if (engine->getCurrentTime() < start || engine->getCurrentTime() > finish)
        return;

    real values[9] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    unsigned int n = 0;

    for (auto t : nodes)
    {
        Mesh *mesh = get<0>(t);
        auto &_nodes = get<1>(t);
        for (auto i: _nodes) {
            CalcNode &node = mesh->getNode(i);
            for (int j = 0; j < 9; j++)
                values[j] += node.values[j];
        }
        n += _nodes.size();
    }

    ofstream out;
    out.open(fileName, ios::out | ios::app);
    out << engine->getCurrentTime();
    for (int j = 0; j < 9; j++)
    {
        out << " " << values[j]/n;
    }
    out << endl;
    out.close();
}