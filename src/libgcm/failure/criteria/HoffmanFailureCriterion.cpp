#include "libgcm/failure/criteria/HoffmanFailureCriterion.hpp"
#include "libgcm/rheology/Failure.hpp"
using namespace gcm;

gcm::HoffmanFailureCriterion::HoffmanFailureCriterion() {
	INIT_LOGGER( "gcm.HoffmanFailureCriterion" );
}

void gcm::HoffmanFailureCriterion::checkFailure(ICalcNode& node, const float tau) {
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
		Zt = Yt,
		Zc = Yc,
		F12 = 1/Xt/Xc + 1/Yt/Yc - 1/Zt/Zc,
		F13 = 1/Xt/Xc - 1/Yt/Yc + 1/Zt/Zc,
		F23 = -1/Xt/Xc + 1/Yt/Yc + 1/Zt/Zc;	
	if ( (s11*s11/(Xt * Xc) + s22 * s22/(Yt * Yc) + s33 * s33/(Zt * Zc) + (s12*s12 + s23*s23 + s13*s13)/(S * S) - F12*s11*s22 - F23*s22*s33 >= 1.0 ))
													            // - F13*s11*s33 + s11/(Xt*Xc) + s22/(Yt*Yc) + s33/(Zt*Zc)) >= 1.0 )
	{
			node.setDestroyed(true);
	}
	
	// if ( ((s11*s12 - s11 * s11)/(Xt * Xc) - s22 * s22/(Yt * Yc) + (Xt+Xc)/(Xt * Xc) * s11 + (Yt+Yc)/(Yt * Yc) * s22 + s12 * s12 / (S * S)) >= 1.0 )
	// {
	// 		node.setDestroyed(true);
	// }

	// //LaRC03(2D)
	// gcm::real Yis = 1.12 * sqrt(2) * Yt;
	// gcm::real Sis = sqrt(2) * S;
	// double g = 0.35; //Коэффициент жесткости разрушения
	// if(g * (s22 / Yis) * (s22 / Yis) + (s12/Sis) + (1 - g) * (s22 / Yis) >= 1.0) 
	// {
	// 	node.setDestroyed(true);
	// }
}
