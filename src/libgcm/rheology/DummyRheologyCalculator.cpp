#include "rheology/DummyRheologyCalculator.h"

#include "node/CalcNode.h"

gcm::DummyRheologyCalculator::DummyRheologyCalculator() {
	INIT_LOGGER("gcm.DummyRheologyCalculator");
}

gcm::DummyRheologyCalculator::~DummyRheologyCalculator() {
	
}

void gcm::DummyRheologyCalculator::doCalc(CalcNode* src, CalcNode* dst)
{
	// do nothing
}
