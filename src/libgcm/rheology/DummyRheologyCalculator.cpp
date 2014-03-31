#include "libgcm/rheology/DummyRheologyCalculator.h"

#include "libgcm/node/CalcNode.h"

gcm::DummyRheologyCalculator::DummyRheologyCalculator() {
    INIT_LOGGER("gcm.DummyRheologyCalculator");
}

void gcm::DummyRheologyCalculator::doCalc(CalcNode& src, CalcNode& dst)
{
    // do nothing
}
