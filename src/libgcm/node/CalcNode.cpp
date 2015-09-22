#include "libgcm/node/CalcNode.hpp"

using namespace gcm;
using std::copy;

CalcNode::CalcNode() : CalcNode(-1)
{
	contactDirection = 0;
}

CalcNode::CalcNode(int num) : CalcNode(num, vector3r())
{
}

CalcNode::CalcNode(int num, const vector3r& coords) : ICalcNode(num, coords)
{
    bodyId = -1;
    memset(values, 0, VALUES_NUMBER * sizeof (real));
    rho = 0;
    materialId = 0;
    publicFlags.flags = 0;
    privateFlags.mainStressCalculated = false;
    errorFlags.flags = 0;
    // Border condition '1' is 'default' one, since '0' is reserved for 'failsafe' one
    borderConditionId = 1;
    contactConditionId = 0;
	contactDirection = 0;
    crackDirection[0] = crackDirection[1] = crackDirection[2] = 0.0;
    damageMeasure = 0.0;
}

CalcNode::CalcNode(const CalcNode& src): ICalcNode(src)
{
    *this = src;
}

CalcNode& CalcNode::operator=(const CalcNode &src)
{
    number = src.number;

    coords = src.coords;
    copy(src.values, src.values + VALUES_NUMBER, values);
    crackDirection = src.crackDirection;
    damageMeasure = src.damageMeasure;

    bodyId = src.bodyId;
    rho = src.rho;
    materialId = src.materialId;
    contactNodeNum = src.contactNodeNum;
    contactDirection = src.contactDirection;
    publicFlags = src.publicFlags;
    privateFlags = src.privateFlags;
    errorFlags = src.errorFlags;
    borderConditionId = src.borderConditionId;
    contactConditionId = src.contactConditionId;
    rheologyMatrix = src.rheologyMatrix;

    return *this;
}

bool CalcNode::operator==(const CalcNode &src) const
{
	return ( fabs(coords[0] == src.coords[0])
				&& coords[1] == src.coords[1]
				&& coords[2] == src.coords[2] );
}

CalcNode::~CalcNode()
{
}

void CalcNode::clearState()
{
    clearErrorFlags();
    clearMainStresses();
}

void CalcNode::clearErrorFlags()
{
    errorFlags.flags = 0;
}

void CalcNode::clearMainStresses()
{
    privateFlags.mainStressCalculated = false;
}

real CalcNode::getCompression() const
{
    real compression = 0;
    real s[3];
    getMainStressComponents(s[0], s[1], s[2]);

    for (int i = 0; i < 3; i++)
        if (s[i] < compression)
            compression = s[i];

    return fabs(compression);
}


real CalcNode::getTension() const
{
    real tension = 0;
    real s[3];
    getMainStressComponents(s[0], s[1], s[2]);

    for (int i = 0; i < 3; i++)
        if (s[i] > tension)
            tension = s[i];

    return tension;
}

// See http://www.toehelp.ru/theory/sopromat/6.html for details
real CalcNode::getShear() const
{
    real shear = 0;
    real s[3];
    real t[3];
    getMainStressComponents(s[0], s[1], s[2]);

    t[0] = 0.5 * fabs(s[1] - s[0]);
    t[1] = 0.5 * fabs(s[2] - s[0]);
    t[2] = 0.5 * fabs(s[2] - s[1]);

    for (int i = 0; i < 3; i++)
        if (t[i] > shear)
            shear = t[i];

    return shear;
}
real CalcNode::getDeviator() const
{
    return sqrt(((sxx - syy) * (sxx - syy) + (syy - szz) * (syy - szz) + (sxx - szz) * (sxx - szz)
                + 6 * (sxy * sxy + sxz * sxz + syz * syz)) / 6);
}

real CalcNode::getPressure() const
{
    real pressure = -(sxx + syy + szz) / 3;
    return pressure;
}

real CalcNode::getJ1() const
{
    return sxx + syy + szz;
}

real CalcNode::getJ2() const
{
    return sxx * syy + sxx * szz + syy * szz - (sxy * sxy + sxz * sxz + syz * syz);
}

real CalcNode::getJ3() const
{
    return sxx * syy * szz + 2 * sxy * sxz * syz - sxx * syz * syz - syy * sxz * sxz - szz * sxy*sxy;
}

void CalcNode::getMainStressComponents(real& s1, real& s2, real& s3) const
{
//    if (!privateFlags.mainStressCalculated)
        calcMainStressComponents();

    s1 = mainStresses[0];
    s2 = mainStresses[1];
    s3 = mainStresses[2];
//    if (s1>1000 || s2>1000 || s3>1000) cout <<endl<<"str: "<<s1<<" "<<s2<<" "<<s3<<" ";
}

// See http://www.toehelp.ru/theory/sopromat/6.html
void CalcNode::calcMainStressComponents() const
{
    real a = -getJ1();
    real b = getJ2();
    real c = -getJ3();
    
    solvePolynomialThirdOrder(a, b, c, mainStresses[0], mainStresses[1], mainStresses[2]);

    privateFlags.mainStressCalculated = true;
}

void CalcNode::calcMainStressDirectionByComponent(real s, vector3r& vector) const
{
    real zero = 0.0000001, a=0,b=0,c=0,u,v,w,n;
//    if (sxx > 400000)	printf("s: %f %f %f %f %f %f   %f ",sxx,sxy,sxz,syy,syz,szz,s);
    if (fabs(sxx-s) <= zero)
    {
	if (fabs(sxy) <= zero)
	{
		if (fabs(sxz) <= zero)
		{
			if (fabs(syy-s) <= zero)
			{
				if (fabs(syz) <= zero)
				{
					if (fabs(szz-s) <= zero)
					{
						a=1.0;
						b=1.0;
						c=1.0;
					}
					else
					{                                                
						a=1.0;
                                                b=1.0;
                                                c=0.0;
					}
				}
				else
				{
                                                a=1.0;
                                                b=0.0;
                                                c=0.0;
				}
			}
			else
			{
				if (fabs((szz-s)*(syy-s)-syz*syz) <= zero)
				{
                                        a=1.0;
                                        b=-syz/(syy-s);
                                        c=1.0;
				}
				else
				{
                                        a=1.0;
                                        b=0.0;
                                        c=0.0;
				}
			}
		}
		else
		{
			if (fabs(syy-s) <= zero)
			{
                                a=-syz/sxz;
                                b=1.0;
                                c=0.0;
			}
			else
			{
                                a=0.0;
                                b=0.0;
                                c=0.0;
			}
		}
	}
	else
	{
		if (fabs(sxz) <= zero)
		{
			if (fabs(szz-s) <= zero)
			{
                                a=-syz/sxy;
                                b=0.0;
                                c=1.0;
			}
			else
			{
                                a=0.0;
                                b=0.0;
                                c=0.0;
			}
		}
		else
		{
			if (fabs(sxz*sxz*(syy-s)-2.0*sxy*syz*sxz+(szz-s)*sxy*sxy) <= zero)
			{
				a = sxz*(syy-s)/sxy/sxy - syz/sxy;
				b = -sxz/sxy;
				c = 1.0;
			}
			else
			{
                                a=0.0;
                                b=0.0;
                                c=0.0;
			}
		}
	}
    }
    else
    {
	if (fabs(sxy) <= zero)	
	{
		if (fabs(sxz) <= zero)
		{
			if (fabs(syy-s) <= zero)
			{
				if (fabs(syz) <= zero)
				{
					if (fabs(szz-s) <= zero)
					{
						a=0.0;
						b=1.0;
						c=1.0;
					}
					else
					{
						a=0.0;
						b=1.0;
						c=0.0;
					}
				}
				else
				{
					a=0.0; 
					b=0.0;
					c=0.0;
				}
			}
			else
			{
				if (fabs((szz-s)*(syy-s)-syz*syz) <= zero)
				{
					a=0.0;
					b=-syz/(syy-s);
					c=1.0;
				}
				else
				{
					a=0.0;
					b=0.0;
					c=0.0;
				}
			}
		}
		else
		{
			if (fabs(sxz*sxz*(syy-s)+(sxx-s)*syz*sxz-(sxx-s)*(syy-s)*(szz-s)) <= zero)
			{
				a=(syy-s)/(sxx-s);
				b=1.0;
				c=-(syy-s)/sxz;
			}
			else
			{
				a=0.0;
				b=0.0;
				c=0.0;
			}
		}
	}
	else
	{
		 u=(sxx-s)*(syy-s)-sxy*sxy;
			 v=(sxx-s)*syz-sxz*sxy;
			 w=syz*(sxx-s)-sxz*sxy;
			 n=(szz-s)*(sxx-s)-sxz*sxz;
		if (fabs(u) <= zero)
		{
			if (fabs(v) <= zero)
			{
				if (fabs(w) <= zero)
				{
					if (fabs(n) <= zero)
					{
						a=-sxy*(sxx-s)-sxz*(sxx-s);
						b=1.0;
						c=1.0;
					}
					else
					{
						a=-sxy/(sxx-s);
						b=1.0;
						c=0.0;
					}
				}
				else
				{
					a=sxy/(sxx-s)*n/w-sxz/(sxx-s);
					b=-n/w;
					c=1.0;
				}
			}
			else
			{
				if (fabs(w) <= zero)
				{
					a=-sxy/(sxx-s);
					b=1.0;
					c=0.0;
				}
				else
				{
					a=0.0;
					b=0.0;
					c=0.0;
				}
			}
		}
		else
		{
//			if (fabs(n*u-w*v) <= zero)
//			{
				a=sxy/(sxx-s)*v/u-sxz/(sxx-s);
				b=-v/u;
				c=1.0;
//			}
//			else
//			{
//				a=0.0;
//				b=0.0;
//				c=0.0;
//			}
		}
	}
    }
    vector[0] = a; vector[1] = b; vector[2] = c;
//    if (sxx > 400000) printf(" vec: %f %f %f  \n %f %f %f %f  nu-wv %f  det %f\n ms %f %f %f \n",vector[0],vector[1],vector[2], u,v,w,n,n*u-w*v,
//		2.0*sxy*sxz*syz+(sxx-s)*(syy-s)*(szz-s)-sxz*(syy-s)*sxz-(sxx-s)*syz*syz-(szz-s)*sxy*sxy,
//		mainStresses[0],mainStresses[1],mainStresses[2]);
    real norm = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
    if (norm > zero) {
        vector[0] /= norm;
        vector[1] /= norm;
        vector[2] /= norm;
    }
    else 
    {
	vector[0] = vector[1] = vector[2] = 0.0;
    }
}

bool CalcNode::rheologyIsValid() const
{
    // FIXME there is no need to check materialId >= 0 since it's uint
    return materialId >= 0 && rho > 0;
}

bool CalcNode::isInContact() const
{
    return publicFlags.contact;
}

void CalcNode::setInContact(bool value)
{
    publicFlags.contact = value;
}

bool CalcNode::isDestroyed() const
{
    return publicFlags.isDestroyed;
}

void CalcNode::setDestroyed(bool value)
{
    publicFlags.isDestroyed = value;
}

bool CalcNode::isContactDestroyed() const
{
    return publicFlags.isContactDestroyed;
}

void CalcNode::setContactDestroyed(bool value)
{
    publicFlags.isContactDestroyed = value;
}

bool CalcNode::isLocal(bool mustBeUsed) const
{
    return (isUsed() || !mustBeUsed) && privateFlags.local;
}

bool CalcNode::isRemote() const
{
    return isUsed() && !isLocal();
}

void CalcNode::setPlacement(bool local)
{
    setUsed(true);
    privateFlags.local = local;
}

bool CalcNode::isUsed() const
{
    return privateFlags.used;
}

void CalcNode::setUsed(bool used)
{
    privateFlags.used = used;
}

bool CalcNode::isFirstOrder() const
{
    return publicFlags.order == 0;
}

bool CalcNode::isSecondOrder() const
{
    return publicFlags.order == 1;
}

void CalcNode::setOrder(uchar order)
{
    switch (order) {
    case 1: publicFlags.order = 0;
        break;
    case 2: publicFlags.order = 1;
        break;
    case 0: THROW_INVALID_ARG("Invalid node order specified");
    default: THROW_UNSUPPORTED("Only first and second order nodes are supported at the moment");
    }
}

void CalcNode::setIsBorder(bool border)
{
    privateFlags.border = border;
}

bool CalcNode::isBorder() const
{
    return privateFlags.border;
}

bool CalcNode::isInner() const
{
    return !isBorder();
}

void CalcNode::setXNeighError()
{
    errorFlags.xNeigh = true;
}

void CalcNode::setYNeighError()
{
    errorFlags.yNeigh = true;
}

void CalcNode::setZNeighError()
{
    errorFlags.zNeigh = true;
}

void CalcNode::setNormalError()
{
    errorFlags.outerNormal = true;
}

void CalcNode::setNeighError(unsigned int axisNum)
{
    switch (axisNum) {
    case 0: setXNeighError();
        break;
    case 1: setYNeighError();
        break;
    case 2: setZNeighError();
        break;
    default: THROW_INVALID_ARG("Invalid axis number specified");
    }
}

void CalcNode::setContactCalculationError()
{
    errorFlags.contactCalculation = true;
}

bool CalcNode::getCustomFlag(uchar flag) const
{
    switch (flag) {
    case FLAG_1: return publicFlags.flag1;
    case FLAG_2: return publicFlags.flag2;
    case FLAG_3: return publicFlags.flag3;
    case FLAG_4: return publicFlags.flag4;
    default: THROW_INVALID_ARG("Invalid flag specified");
    }
}

void CalcNode::setCustomFlag(uchar flag, bool value)
{
    switch (flag) {
    case FLAG_1: publicFlags.flag1 = value; break;
    case FLAG_2: publicFlags.flag2 = value; break;
    case FLAG_3: publicFlags.flag3 = value; break;
    case FLAG_4: publicFlags.flag4 = value; break;
    default: THROW_INVALID_ARG("Invalid flag specified");
    }
}

void CalcNode::setBorderConditionId(uchar newBorderCondId)
{
    borderConditionId = newBorderCondId;
}

uchar CalcNode::getBorderConditionId() const
{
    return borderConditionId;
}

void CalcNode::setContactConditionId(uchar newContactConditionId)
{
    contactConditionId = newContactConditionId;
}

uchar CalcNode::getContactConditionId() const
{
    return contactConditionId;
}

void CalcNode::setMaterialId(uchar id)
{
    materialId = id;
    rho = Engine::getInstance().getMaterial(id)->getRho();
}

uchar CalcNode::getMaterialId() const
{
    return materialId;
}

void CalcNode::setRheologyMatrix(RheologyMatrixPtr matrix)
{
    assert_true(matrix.get());
    rheologyMatrix = matrix;
}

RheologyMatrixPtr CalcNode::getRheologyMatrix() const
{
    return rheologyMatrix;
}

MaterialPtr CalcNode::getMaterial() const
{
    return Engine::getInstance().getMaterial(materialId);
}

void CalcNode::setRho(real rho)
{
    this->rho = rho;
}

real CalcNode::getRho() const
{
    return rho;
}

real CalcNode::getRho0() const
{
    return Engine::getInstance().getMaterial(materialId)->getRho();
}

void CalcNode::setDamageMeasure(real value)
{
    this->damageMeasure = value;
}

real CalcNode::getDamageMeasure() const
{
    return damageMeasure;
}

const vector3r& CalcNode::getCrackDirection() const
{
    return crackDirection;
}

void CalcNode::createCrack(int direction)
{
    if (crackDirection*crackDirection == 0.0)
        this->calcMainStressDirectionByComponent(mainStresses[direction], crackDirection);
}

void CalcNode::createCrack(const vector3r& crack)
{
    crackDirection = crack;
}

void CalcNode::createCrack(real x, real y, real z)
{
    crackDirection[0] = x;
    crackDirection[1] = y;
    crackDirection[2] = z;
}

void CalcNode::exciseByCrack()
{
    if (crackDirection*crackDirection != 0.0)
        cleanStressByDirection(getCrackDirection());
}
void CalcNode::cleanStressByDirection(const vector3r& h)
{
    real s1 = h[0]*(sxx * h[0] + sxy * h[1] + sxz * h[2]) + h[1]*(sxy * h[0] + syy * h[1] + syz * h[2]) + h[2]*(sxz * h[0] + syz * h[1] + szz * h[2]); //TODO
    if (s1 < 0) return;
    sxx -= h[0] * h[0] * s1;
    sxy -= h[0] * h[1] * s1;
    sxz -= h[0] * h[2] * s1;
    syy -= h[1] * h[1] * s1;
    syz -= h[1] * h[2] * s1;
    szz -= h[2] * h[2] * s1;
//    cout<<endl<<"vec: "<<h[0]<<" "<<h[1]<<" "<<h[2]<<"  "<<s1;
}

uchar CalcNode::getPrivateFlags() const
{
    return privateFlags.flags;
}

void CalcNode::setPrivateFlags(uchar flags)
{
    privateFlags.flags = flags;
}

uchar CalcNode::getPublicFlags() const
{
    return publicFlags.flags;
}

void CalcNode::setPublicFlags(uchar flags)
{
    publicFlags.flags = flags;
}

uchar CalcNode::getErrorFlags() const
{
    return errorFlags.flags;
}

void CalcNode::setErrorFlags(uchar flags)
{
    errorFlags.flags = flags;
}
