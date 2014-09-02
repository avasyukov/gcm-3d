#include "libgcm/failure/criteria/MaxStressFailureCriterion.hpp"

gcm::MaxStressFailureCriterion::MaxStressFailureCriterion() {
    INIT_LOGGER( "gcm.MaxStressFailureCriterion" );
}

void gcm::MaxStressFailureCriterion::checkFailure(ICalcNode& node, const float tau) {
    if( node.isDestroyed() )
        return;
    real stresses[3];
    node.getMainStressComponents(stresses[0], stresses[1], stresses[2]);
    MaterialPtr mat = node.getMaterial();
    real limit = mat->getCrackThreshold();
    for(real s : stresses) {
        if( s > limit) {
            node.setDestroyed(true);
        }
    }
}
