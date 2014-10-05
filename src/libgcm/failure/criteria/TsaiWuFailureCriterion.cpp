#include "libgcm/failure/criteria/TsaiWuFailureCriterion.hpp"
#include "libgcm/rheology/Failure.hpp"

using namespace gcm;

gcm::TsaiWuFailureCriterion::TsaiWuFailureCriterion() {
	INIT_LOGGER( "gcm.TsaiWuFailureCriterion" );
}

void gcm::TsaiWuFailureCriterion::checkFailure(ICalcNode& node, const float tau) {
	if ( node.isDestroyed() )
		return;
	MaterialPtr mat = node.getMaterial();
	auto props = mat->getFailureProperties();
	int dir = props[FAILURE_TYPE_HASHIN][FAILURE_TYPE_HASHIN_DIR];
	real Xc = props[FAILURE_TYPE_HASHIN][FAILURE_TYPE_HASHIN_XC];
	real Xt = props[FAILURE_TYPE_HASHIN][FAILURE_TYPE_HASHIN_XT];
	real Yc = props[FAILURE_TYPE_HASHIN][FAILURE_TYPE_HASHIN_YC];
	real Yt = props[FAILURE_TYPE_HASHIN][FAILURE_TYPE_HASHIN_YT];
	real St = props[FAILURE_TYPE_HASHIN][FAILURE_TYPE_HASHIN_ST];
	real S = props[FAILURE_TYPE_HASHIN][FAILURE_TYPE_HASHIN_S];
	gcm::real s11, s12, s13, s22, s23, s33;
	switch (dir)
	{
		case 1:
			s11 = node.szz;
			s12 = node.sxz;
			s13 = node.syz;
			s22 = node.syy;
			s23 = node.sxy;
			s33 = node.sxx;
			break;
		case 3:
			s11 = node.syy;
			s12 = node.sxy;
			s13 = node.syz;
			s22 = node.sxx;
			s23 = node.sxz;
			s33 = node.szz;
			break;
		case 2:
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
	gcm::real F2 = 1/Yt - 1/Yc,
		F3 = 1/Xt - 1/Xc,
		F22 = 1/Yt/Yc,
		F33 = 1/Xt/Xc,
		F44 = 1/S/S,
		F12 = -0.5/Yt/Yc,
		F23 = -0.5*sqrt(1/Yt/Yc/Xt/Xc),
		F66 = 2*(F22-F12);
	if ( F2*(s11+s22) + F3*s33 + F22*(s11*s11 + s22*s22) + F33*s33*s33 + F44*(s23*s23 + s13*s13) + F66*s12*s12 + 2*F12*s11*s22 + 2*F23*(s11+s22)*s33 >= 1.0 )
	{
		node.setDestroyed(true);
		//node.setDamageMeasure(4);
	}
}
