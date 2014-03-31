#include "libgcm/method/DummyMethod.hpp"

#include "libgcm/node/CalcNode.hpp"

gcm::DummyMethod::DummyMethod()
{
    INIT_LOGGER("gcm.method.DummyMethod");
}

string gcm::DummyMethod::getType()
{
    return "DummyMethod";
}

int gcm::DummyMethod::getNumberOfStages()
{
    return 1;
}

void gcm::DummyMethod::doNextPartStep(CalcNode& cur_node, CalcNode& new_node, float time_step, int stage, Mesh* mesh)
{
    for(int i = 0; i < 9; i++)
        new_node.values[i] = cur_node.values[i];
}
