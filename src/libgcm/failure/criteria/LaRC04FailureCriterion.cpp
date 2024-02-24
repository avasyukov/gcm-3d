#include <math.h>
#include "libgcm/failure/criteria/LaRC04FailureCriterion.hpp"
#include "libgcm/rheology/Failure.hpp"

using namespace gcm;


LaRC04FailureCriterion::LaRC04FailureCriterion() {
    INIT_LOGGER( "gcm.LaRC03FailureCriterion" );
}

void LaRC04FailureCriterion::checkFailure(ICalcNode& node, const float tau) {
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
	double g = 0.7; //Fracture stiffness coefficient
    double G12 = 43.7; //Shift modulus, GPa
    double E1 = 133.76; //GPa
    double E2 = 8.05; //GPa
    double nu21 = 0.32; //Poisson's coefficient

    double lambda23 = 2 * (1 / E2 - nu21 * nu21 / E1);

    double a0 = 53; //angle from test data
    double etaT = -1 / tan(2 * a0);
    double etaL = -Sis * cos(2* a0) / (Yc * cos(a0) * cos(a0));  //approximate

    double ST = Yc * cos(a0) * sin(a0 + cos(a0) / tan(2 * a0));

    double psi = atan(2 * s23 / (s22 + s33)) / 2 * 57.3; 

    double phiC = atan(1 - sqrt((1 - 4 * (Sis/Xc + etaL) * (Sis / Xc)) / (2 * (Sis/Xc + etaL)))) * 57.3;

    double s_2psi_2psi = (s22 + s33) / 2 + (s22 + s33) * cos(2 * psi) / 2 + s23 * sin(2 * psi);
    double s_3psi_3psi = s22 + s33 - 2 * s_2psi_2psi;
    double tau_12psi = s12 * cos(psi) + s13 * sin(psi);
    double tau_2psi_3psi = 0;
    double tau_3psi_1 = s13 * cos(psi) - s12 * sin(psi);

    double gamma_1m2m = phiC * Xc / G12;
    double phi0 = phiC - gamma_1m2m;
    double phi = tau_12psi * (phi0 + gamma_1m2m) / abs(tau_12psi);

    double s_1m1m = (s11 + s_2psi_2psi) / 2 + (s11 - s_2psi_2psi) * cos(2 * phi) / 2 + tau_12psi * sin(2 * phi);
    double s_2m2m = s11 + s_2psi_2psi - s_1m1m;
    double tau_1m2m = -(s11 - s_2psi_2psi) * cos(2 * phi) + tau_12psi * cos(2 * phi);
    double tau_2m_3psi = tau_2psi_3psi * cos(phi) - tau_3psi_1 * sin(phi);

    double s_mn = (s_2m2m + s_3psi_3psi) / 2 + (s_2m2m - s_3psi_3psi) * cos(2 * a0) / 2 +  tau_2m_3psi * sin(2 * a0);
    double tau_Tm = - (s_2m2m - s_3psi_3psi) * sin(2 * a0) + tau_2m_3psi * cos(2 * a0);
    double tau_Lm = tau_1m2m * cos(a0) + tau_2psi_3psi * cos(2 * a0);

    double s_n = (s22 + s33) / 2 + (s22 - s33) * cos(2 * a0) / 2 + s23 * sin(2 * a0);
    double tau_T = (s22 + s33) / 2 - (s22 - s33) * sin(2 * a0) / 2 + s23 * cos(2 * a0);
    double tau_L = s12 * cos(a0) + s13 * sin(a0);

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
    	if ( s_2m2m < 0 )
    	{
            if ( (tau_1m2m / (Sis - etaL * s_2m2m)) * (tau_1m2m / (Sis - etaL * s_2m2m)) > 1.0 ) 
            {
        	    node.setDestroyed(true);
//		    	node.setDamageMeasure(2);
            }
		}
        else
        {
            if ( (1 - g) * s_2m2m / Yis + g * s_2m2m * s_2m2m / (Yis * Yis) + tau_1m2m * tau_1m2m / (S * S) + lambda23 * tau_2m_3psi * tau_2m_3psi / (S * S * 2) > 1.0 ) 
            {
        	    node.setDestroyed(true);
//		    	node.setDamageMeasure(2);
            }
        }
    }

//  matrix cracking
    if ( s22 >= 0.0 ) 
    {
	    if ( (1 - g) * s22 / Yis + g * (s22 / Yis) * (s22 / Yis) + s12 * s12 / (S * S) + lambda23 * s23 * s23 / (S * S * 2 ) > 1.0 )  
	    {   
		    node.setDestroyed(true);
//			node.setDamageMeasure(3);
	    }
    }
    else
    {
        if ( s11 < -Yc)  
        {
            if( (tau_Tm / (ST - etaT * s_mn)) * (tau_Tm / (ST - etaT * s_mn)) + (tau_Lm / (Sis - etaL * s_mn)) * (tau_Lm / (Sis - etaL * s_mn)) > 1.0 )
            node.setDestroyed(true);
//		    node.setDamageMeasure(4);
        }
        else
        {
            if ( (tau_T / (ST - etaT * s_n)) * (tau_T / (ST - etaT * s_n)) + (tau_L / (Sis - etaL * s_n)) * (tau_L / (Sis - etaL * s_n))  > 1.0 )  
	        {   
		        node.setDestroyed(true);
//			    node.setDamageMeasure(3);
	        }
        }
    }
}
