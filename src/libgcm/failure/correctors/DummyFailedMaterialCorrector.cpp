#include "libgcm/failure/correctors/DummyFailedMaterialCorrector.hpp"

gcm::DummyFailedMaterialCorrector::DummyFailedMaterialCorrector() {
    INIT_LOGGER( "gcm.DummyFailedMaterialCorrector" );
}

void gcm::DummyFailedMaterialCorrector::applyCorrection(ICalcNode& node) {
    // Do nothing, failed material behaves exactly as non-failed one.
    // Never ever use this implementation for real tasks, it is meant for debugging purposes only.
}
