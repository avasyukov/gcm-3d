#include "libgcm/failure/criteria/DruckerPragerFailureCriterion.hpp"
#include "libgcm/rheology/Failure.hpp"

using namespace gcm;


DruckerPragerFailureCriterion::DruckerPragerFailureCriterion() {
    INIT_LOGGER( "gcm.DruckerPragerFailureCriterion" );
}

void DruckerPragerFailureCriterion::checkFailure(ICalcNode& node, const float tau) {
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
	break;
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

    real	S1 = (Xc + Xt)/(2*Xc*Xt),
		S2 = (Yc + Yt)/(2*Yc*Yt);
    real	S3 = S2,
		F = 0.5*(S2*S2 + S3*S3 - S1*S1),
		G = 0.5*(S1*S1 + S3*S3 - S2*S2),
		H = 0.5*(S2*S2 + S1*S1 - S3*S3),
		I = (Xc - Xt)/(2*Xt*Xc),
		J = (Yc - Yt)/(2*Yt*Yc);
    real	K = J,
		L = 1/(2*St*St),
		M = 1/(2*S*S);
    real	N = M;
    real	f = sqrt(F*(s22-s33)*(s22-s33) + G*(s33-s11)*(s33-s11) + H*(s11-s22)*(s11-s22) + 2*L*s23*s23 + 2*M*s13*s13 + 2*N*s12*s12) + I*s11 + J*s22 + K*s33 - 1;

    if (f > 0)
    {
	node.setDestroyed(true);
    }
}
