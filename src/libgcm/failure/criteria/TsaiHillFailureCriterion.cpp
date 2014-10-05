#include "libgcm/failure/criteria/TsaiHillFailureCriterion.hpp"
#include "libgcm/rheology/Failure.hpp"
using namespace gcm;

gcm::TsaiHillFailureCriterion::TsaiHillFailureCriterion() {
	INIT_LOGGER( "gcm.TsaiHillFailureCriterion" );
}

void gcm::TsaiHillFailureCriterion::checkFailure(ICalcNode& node, const float tau) {
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
	real S = props[FAILURE_TYPE_HASHIN][FAILURE_TYPE_HASHIN_S];
	gcm::real s11, s12, s13, s22, s23, s33;
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
	gcm::real X = (Xc + Xt)/2.0,
		Y = (Yc + Yt)/2.0,
		Z = Y,
		F = (1/Y/Y + 1/Z/Z - 1/X/X)/2.0,
		G = (1/Z/Z + 1/X/X - 1/Y/Y)/2.0,
		H = (1/X/X + 1/Y/Y - 1/Z/Z)/2.0,
		L = 1/St/St/2.0,
		M = 1/S/S/2.0,
		N = M;
	if ( (F*(s22-s33)*(s22-s33) + G*(s33-s11)*(s33-s11) + H*(s11-s22)*(s11-s22) + 2*L*s23 + 2*M*s12 + 2*N*s13)/2.0 >= 1.0 )
	{
		node.setDestroyed(true);
		//node.setDamageMeasure(4);
	}
}
