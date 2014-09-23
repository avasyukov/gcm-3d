#include "libgcm/failure/criteria/HashinFailureCriterion.hpp"
#include "libgcm/rheology/Failure.hpp"

using namespace gcm;


HashinFailureCriterion::HashinFailureCriterion() {
    INIT_LOGGER( "gcm.HashinFailureCriterion" );
}

void HashinFailureCriterion::checkFailure(ICalcNode& node, const float tau) {
    if( node.isDestroyed() )
        return;
    MaterialPtr mat = node.getMaterial();
    auto props = mat->getFailureProperties();
    int dir = props[FAILURE_TYPE_HASHIN][FAILURE_TYPE_HASHIN_DIR];
    real Xc = props[FAILURE_TYPE_HASHIN][FAILURE_TYPE_HASHIN_XC];
    real Xt = props[FAILURE_TYPE_HASHIN][FAILURE_TYPE_HASHIN_XT];
    real Yc = props[FAILURE_TYPE_HASHIN][FAILURE_TYPE_HASHIN_YC];
    real Yt = props[FAILURE_TYPE_HASHIN][FAILURE_TYPE_HASHIN_YT];
    real St = props[FAILURE_TYPE_HASHIN][FAILURE_TYPE_HASHIN_ST];
    real S  = props[FAILURE_TYPE_HASHIN][FAILURE_TYPE_HASHIN_S];
    real s11, s12, s13, s22, s23, s33;
    switch (dir)
    {
    case 1:
	s11 = node.sxx;
	s12 = node.sxy;
	s13 = node.sxz;
	s22 = node.syy;
	s23 = node.syz;
	s33 = node.szz;
	break;
    case 2:   
        s11 = node.syy;
        s12 = node.sxy;
        s13 = node.syz;
        s22 = node.sxx;
        s23 = node.sxz;
        s33 = node.szz;
    case 3:
        s11 = node.szz;
        s12 = node.sxz;
        s13 = node.syz;
        s22 = node.sxx;
        s23 = node.sxy;
        s33 = node.syy;
        break;
    default:
		return;
    }

    if (s11 > 0.0)
    {
    	if ( (s11/Xt)*(s11/Xt) + (s12*s12 + s13*s13)/(S*S) > 1.0)
		{
			node.setDestroyed(true);
			node.setDamageMeasure(1);
		}
    }
    else 
    {
    	if ( (s11*s11)/(Xc*Xc) > 1.0)
    	{
        	node.setDestroyed(true);
			node.setDamageMeasure(2);
		}
    }

/*    if (s33 < 0.0)
    	if ( (s33*s33)/(Zc*Zc) > 1.0)
        	node.setDestroyed(true); */

    if (s22+s33 > 0.0)
    {
    	if ( ((s22+s33)/Yt)*((s22+s33)/Yt) + (s23*s23-s22*s33)/(St*St) + (s12*s12+s13*s13)/(S*S) > 1.0)
    	{
        	node.setDestroyed(true);
			node.setDamageMeasure(3);
		}
    }
    else
    {
    	if ((Yc/(2*St)-1)*(Yc/(2*St)-1)*((s22+s33)/Yc) + ((s22+s33)/(4*St))*((s22+s33)/(4*St)) + (s23*s23-s22*s33)/(St*St) + (s12*s12+s13*s13)/(S*S) > 1.0)
    	{
        	node.setDestroyed(true);
			node.setDamageMeasure(4);
        }
    }
}
