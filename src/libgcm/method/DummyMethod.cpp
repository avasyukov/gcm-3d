#include "method/DummyMethod.h"

#include "node/CalcNode.h"

gcm::DummyMethod::DummyMethod()
{
	INIT_LOGGER("gcm.method.DummyMethod");
}

gcm::DummyMethod::~DummyMethod()
{
}

string gcm::DummyMethod::getType()
{
	return "DummyMethod";
}

int gcm::DummyMethod::getNumberOfStages()
{
	return 1;
}

float gcm::DummyMethod::getMaxLambda(CalcNode* node)
{
	assert( node != NULL );
	assert ( node->rheologyIsValid() );
	return sqrt( ( (node->getLambda()) + 2 * (node->getMu()) ) / (node->getRho()) );
}

void gcm::DummyMethod::doNextPartStep(CalcNode& cur_node, CalcNode& new_node, float time_step, int stage, Mesh* mesh)
{
	for(int i = 0; i < 9; i++)
		new_node.values[i] = cur_node.values[i];
}
