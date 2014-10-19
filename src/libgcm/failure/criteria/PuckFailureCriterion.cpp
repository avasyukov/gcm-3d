#include "libgcm/failure/criteria/PuckFailureCriterion.hpp"
#include "libgcm/rheology/Failure.hpp"

using namespace gcm;


PuckFailureCriterion::PuckFailureCriterion() {
    INIT_LOGGER( "gcm.PuckFailureCriterion" );
}

void PuckFailureCriterion::checkFailure(ICalcNode& node, const float tau) {
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
    int dir1 = 0, dir2 = 0;
    switch (dir)
    {
    case 1:
	s11 = node.sxx;
	s12 = node.sxy;
	s13 = node.sxz;
	s22 = node.syy;
	s23 = node.syz;
	s33 = node.szz;
	dir1 = 1;
	dir2 = 2;
	break;
    case 2:   
        s11 = node.syy;
        s12 = node.syz;
        s13 = node.sxy;
        s22 = node.szz;
        s23 = node.sxz;
        s33 = node.sxx;
	dir1 = 2;
	dir2 = 0;
	break;
    case 3:
        s11 = node.szz;
        s12 = node.sxz;
        s13 = node.syz;
        s22 = node.sxx;
        s23 = node.sxy;
        s33 = node.syy;
	dir1 = 0;
	dir2 = 1;
        break;
    default:
		return;
    }

    real m_s[3] = {0};
//FF
    if (s11 > 0 && s11/Xt - 1 > 0)
    {
	m_s[dir-1] = 1;
        node.createCrack(m_s[0],m_s[1],m_s[2]);
        node.setDestroyed(true);
	node.setDamageMeasure(1);
	return;
    }
    if (s11 < 0 && -s11/Xc - 1 > 0)
    {
        m_s[dir-1] = 1;
        node.createCrack(m_s[0],m_s[1],m_s[2]);
        node.setDestroyed(true);
	node.setDamageMeasure(2);
        return;
    }
//IFF
    real	RATII = S,
		pTII_minus = 0.000003,
		pTII_plus = 0.000003,
		pTT_minus = 0.000003,
		pTT_plus = 0.000003,
		RATT = Yc/((1+pTT_minus)),
		RAT_plus = Yt;
//--- Mode A
 /*   real	f = sqrt((s23/RATII)*(s23/RATII) + (1-pTII_plus*RAT_plus/RATII)*(1-pTII_plus*RAT_plus/RATII) * (s22/RAT_plus)*(s22/RAT_plus)) + pTII_plus*s22/RATII - 1;
    if (s22 > 0 && f > 0)
    {
	m_s[dir1] = 1;
        node.createCrack(m_s[0],m_s[1],m_s[2]);
       	node.setDestroyed(true);
	node.setDamageMeasure(3);
	return;
    }
//--- Mode B
    f = sqrt((s23/RATII)*(s23/RATII) - (pTII_plus/RATII)*(pTII_plus/RATII)*s22*s22) + (pTII_plus/RATII)*s22 - 1;
    if (s22 < 0 && f > 0)
    {
        m_s[dir2] = 1;
        node.createCrack(m_s[0],m_s[1],m_s[2]);
        node.setDestroyed(true);
	node.setDamageMeasure(4);
        return;
    }*/
//--- Mode C
    real 	s1 = (s22+s33)/2 + sqrt(((s22-s33)/2)*((s22-s33)/2) + s23*s23),
		s2 = (s22+s33)/2 - sqrt(((s22-s33)/2)*((s22-s33)/2) + s23*s23);
    real	t12 = (s1-s2)/2;
    real 	cosO = sqrt(((t12/s2)*(t12/s2) * (RATT/S)*(RATT/S) + 1)/(2*(1+pTT_minus)));
    if (s23 > 0) cosO = -cosO;	//plus
    //if (s23 < 0) cosO = -cosO;  //minus
    real	sn = s22*cosO*cosO + s33*(1-cosO*cosO) + 2*s23*cosO*sqrt(1-cosO*cosO),
		tnt = (s33-s22)*cosO*sqrt(1-cosO*cosO) + s23*(2*cosO*cosO - 1),
		tnl = s13*sqrt(1-cosO*cosO) + s12*cosO;
    real	cos2psi = tnt*tnt/(tnt*tnt + tnl*tnl),
		sin2psi = tnl*tnl/(tnt*tnt + tnl*tnl);
    real	pR_plus = pTT_plus*cos2psi/RATT + pTII_plus*sin2psi/RATII,
		pR_minus = pTT_minus*cos2psi/RATT + pTII_minus*sin2psi/RATII;
    real	f_plus = sqrt(( (1/RAT_plus-pR_plus)*sn )*( (1/RAT_plus-pR_plus)*sn ) + (tnt/RATT)*(tnt/RATT) + (tnl/RATII)*(tnl/RATII)) + pR_plus*sn - 1,
		f_minus = sqrt((tnt/RATT)*(tnt/RATT) + (tnl/RATII)*(tnl/RATII) + (pR_minus*sn)*(pR_minus*sn)) + pR_minus*sn - 1;
    if (sn >= 0 && f_plus > 0) 
    {
	m_s[dir] = 0;
	m_s[dir1] = cosO;
	m_s[dir2] = sqrt(1 - cosO*cosO);
        node.createCrack(m_s[0],m_s[1],m_s[2]);
        node.setDestroyed(true);
	node.setDamageMeasure(5);
        return;
    }
    if (sn < 0 && f_minus > 0)
    {   
        m_s[dir] = 0;
        m_s[dir1] = cosO;
        m_s[dir2] = sqrt(1 - cosO*cosO);
        node.createCrack(m_s[0],m_s[1],m_s[2]);
        node.setDestroyed(true);
	node.setDamageMeasure(6);
        return;
    }

}
