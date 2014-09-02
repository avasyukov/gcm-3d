#include "libgcm/failure/criteria/MaxStressCrackFailureCriterion.hpp"

gcm::MaxStressCrackFailureCriterion::MaxStressCrackFailureCriterion() {
    INIT_LOGGER( "gcm.MaxStressCrackFailureCriterion" );
}

void gcm::MaxStressCrackFailureCriterion::checkFailure(ICalcNode& node, const float tau) {
    
    real m_s[3];
    node.getMainStressComponents(m_s[0], m_s[1], m_s[2]);
    int i_ms=0; if (m_s[1]>m_s[i_ms]) i_ms=1; if (m_s[2]>m_s[i_ms]) i_ms = 2;
    if (m_s[i_ms] > node.getMaterial()->getCrackThreshold())
        node.createCrack(i_ms);
}
