#include "libgcm/rheology/DummyRheologyCalculator.hpp"

#include "libgcm/node/CalcNode.hpp"

gcm::DummyRheologyCalculator::DummyRheologyCalculator() {
    INIT_LOGGER("gcm.DummyRheologyCalculator");
}

void gcm::DummyRheologyCalculator::doCalc(CalcNode& src, CalcNode& dst)
{
    // do nothing
}
