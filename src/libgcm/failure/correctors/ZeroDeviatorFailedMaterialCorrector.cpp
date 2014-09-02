#include "libgcm/failure/correctors/ZeroDeviatorFailedMaterialCorrector.hpp"

gcm::ZeroDeviatorFailedMaterialCorrector::ZeroDeviatorFailedMaterialCorrector() {
    INIT_LOGGER( "gcm.ZeroDeviatorFailedMaterialCorrector" );
}

void gcm::ZeroDeviatorFailedMaterialCorrector::applyCorrection(ICalcNode& node) {
    real p = (node.sxx + node.syy + node.szz) / 3;
    node.sxx = node.syy = node.szz = p;
    node.sxy = node.sxz = node.syz = 0;
}
