#include "Sensor.hpp"

#include "libgcm/Engine.hpp"
#include "libgcm/mesh/Mesh.hpp"

using namespace gcm;
using std::string;
using std::vector;
using std::tuple;
using std::get;

Sensor::Sensor(const string& name, const Area* area, Engine* engine): name(name), area(area), engine(engine)
{
    INIT_LOGGER("gcm.plugins.ndi.Sensor");

    for (int i = 0; i < engine->getNumberOfBodies(); i++ )
    {
        Mesh* mesh = engine->getBody(i)->getMeshes();

        nodes.push_back(tuple<Mesh*,vector<int>>(mesh, {}));

        auto& sensor_nodes = get<1>(nodes.back());

        for (int i = 0; i < mesh->getNodesNumber(); i++)
        {
            CalcNode& node = mesh->getNode(i);
            if (node.isUsed() && area->isInArea(node))
                sensor_nodes.push_back(i);
        }
    }
}

Sensor::~Sensor()
{

}

const string& Sensor::getName() const
{
    return name;
}