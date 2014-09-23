#include "libgcm/failure/correctors/CrackResponseFailedMaterialCorrector.hpp"

using namespace gcm;

CrackResponseFailedMaterialCorrector::CrackResponseFailedMaterialCorrector() {
    INIT_LOGGER( "gcm.CrackResponseFailedMaterialCorrector" );
}

void CrackResponseFailedMaterialCorrector::applyCorrection(ICalcNode& node, const float tau) {
    node.exciseByCrack();
}
