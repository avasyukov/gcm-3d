#include <math.h>
#include "libgcm/failure/criteria/LaRC03FailureCriterion.hpp"
#include "libgcm/rheology/Failure.hpp"

using namespace gcm;


LaRC03FailureCriterion::LaRC03FailureCriterion() {
    INIT_LOGGER( "gcm.LaRC03FailureCriterion" );
}

void LaRC03FailureCriterion::checkFailure(ICalcNode& node, const float tau) {
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

    gcm::real Yis = 1.12 * sqrt(2) * Yt;
	gcm::real Sis = sqrt(2) * S;
	double g = 0.9; //Fracture stiffness coefficient
    double G12 = 79.3; //Shift modulus, GPa

    double a0 = 53; //angle from test data
    double etaT = -1 / tan(2 * a0);
    double etaL = -Sis * cos(2* a0) / (Yc * cos(a0) * cos(a0));  //approximate

    double ST = Yc * cos(a0) * sin(a0 + cos(a0) / tan(2 * a0));

    double tauEffT = - s22 * cos(a0) * (sin(a0) - etaT * cos(a0));
    double tauEffL = cos(a0) * (abs(s12) + etaL * s22 * cos(a0));

    double phiC = atan(1 - sqrt((1 - 4 * (Sis/Xc + etaL) * (Sis / Xc)) / (2 * (Sis/Xc + etaL)))) * 57.3;
    double phi = (abs(s12) + (G12 - Xc) * phiC) / (G12 + s11 - s22) * 57.3;


    double s11m = cos(phi) * cos(phi) * s11 + sin(phi) * sin(phi) * s22 + 2 * sin(phi) * cos(phi) * s12;
    double s22m = sin(phi) * sin(phi) * s11 + cos(phi) * cos(phi) * s22 - 2 * sin(phi) * cos(phi) * s12;
    double s12m = -sin(phi) * cos(phi) * s11 + sin(phi) * cos(phi) *s22 + (cos(phi) * cos(phi) - sin(phi) * sin(phi)) * s12;

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

//  fiber failure
    if ( s11 > 0.0 )
    {
    	if ( s11/Xt > 1.0 )
		{
			node.setDestroyed(true);
//			node.setDamageMeasure(1);
		}
    }
    else 
    {
    	if ( s22m < 0 )
    	{
            if ( (abs(s12m) + etaL * s22m) / Sis > 1.0) 
            {
        	    node.setDestroyed(true);
//		    	node.setDamageMeasure(2);
            }
		}
        else
        {
            if ( g * (s22 / Yis) * (s22 / Yis) + (s12/Sis) + (1 - g) * (s22 / Yis) > 1.0 ) 
            {
        	    node.setDestroyed(true);
//		    	node.setDamageMeasure(2);
            }
        }
    }


//  matrix cracking
    if ( s22 > 0.0 ) 
    {
	    if ( g * (s22 / Yis) * (s22 / Yis) + (s12/Sis) + (1 - g) * (s22 / Yis) > 1.0 )  
	    {   
		    node.setDestroyed(true);
//			node.setDamageMeasure(3);
	    }
    }
    else
    {
        if ( (tauEffT/ST) * (tauEffL/ST) + (tauEffT/Sis) * (tauEffL/Sis) > 1.0 ) 
        {
            node.setDestroyed(true);
//		    node.setDamageMeasure(4);
        }
    }
}
