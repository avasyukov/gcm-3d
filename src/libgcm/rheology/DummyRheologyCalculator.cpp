#include "libgcm/rheology/DummyRheologyCalculator.hpp"

#include "libgcm/node/CalcNode.hpp"

using namespace gcm;

DummyRheologyCalculator::DummyRheologyCalculator() {
    INIT_LOGGER("gcm.DummyRheologyCalculator");
}

void DummyRheologyCalculator::doCalc(CalcNode& src, CalcNode& dst)
{
    // do nothing
}
