#include "libgcm/failure/criteria/MaxStressFailureCriterion.hpp"
#include "libgcm/rheology/Failure.hpp"

using namespace gcm;


MaxStressFailureCriterion::MaxStressFailureCriterion() {
    INIT_LOGGER( "gcm.MaxStressFailureCriterion" );
}

void MaxStressFailureCriterion::checkFailure(ICalcNode& node, const float tau) {
    if( node.isDestroyed() )
        return;
    real stresses[3];
    node.getMainStressComponents(stresses[0], stresses[1], stresses[2]);
    MaterialPtr mat = node.getMaterial();
    auto props = mat->getFailureProperties();
    real limit = props[FAILURE_TYPE_MAX_STRESS][FAILURE_TYPE_MAX_STRESS_THRESHOLD];
    for(real s : stresses) {
        if( s > limit) {
            node.setDestroyed(true);
        }
    }
}
