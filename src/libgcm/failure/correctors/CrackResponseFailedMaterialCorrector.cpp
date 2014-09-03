#include "libgcm/failure/correctors/CrackResponseFailedMaterialCorrector.hpp"

gcm::CrackResponseFailedMaterialCorrector::CrackResponseFailedMaterialCorrector() {
    INIT_LOGGER( "gcm.CrackResponseFailedMaterialCorrector" );
}

void gcm::CrackResponseFailedMaterialCorrector::applyCorrection(ICalcNode& node, const float tau) {
    node.exciseByCrack();
}
