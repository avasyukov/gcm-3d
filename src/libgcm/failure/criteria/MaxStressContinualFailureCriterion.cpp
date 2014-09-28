#include "MaxStressContinualFailureCriterion.hpp"
#include "libgcm/rheology/Failure.hpp"

using namespace gcm;

MaxStressContinualFailureCriterion::MaxStressContinualFailureCriterion() {
    INIT_LOGGER( "gcm.MaxStressContinualFailureCriterion" );
}

void MaxStressContinualFailureCriterion::checkFailure(ICalcNode& node, const float tau) {
    if ( node.isDestroyed() ) return;
    else {
        real m_s[3];
        node.getMainStressComponents(m_s[0], m_s[1], m_s[2]);
        int i_ms=0; 
        if (fabs(m_s[1])>fabs(m_s[i_ms])) i_ms = 1; 
        if (fabs(m_s[2])>fabs(m_s[i_ms])) i_ms = 2;
        MaterialPtr mat = node.getMaterial();
        auto props = mat->getFailureProperties();
        real limit = props[FAILURE_TYPE_MAX_STRESS][FAILURE_TYPE_MAX_STRESS_THRESHOLD];
        real s_rel = fabs(m_s[i_ms])/limit;
        real damage = node.getDamageMeasure();
        if ( ( s_rel > 1 ) && (m_s[i_ms] > 0) ) {
            // Parameters of damage evolution
            real A = 100;
            damage += tau * A * s_rel / (1 - damage);
            if ( damage >= 1 ) {
                node.setDestroyed(true);
                damage = 1;
            }
        }
        node.setDamageMeasure(damage);
    }
}
