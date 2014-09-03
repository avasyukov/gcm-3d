#include "libgcm/node/CalcNode.hpp"

gcm::CalcNode::CalcNode() : CalcNode(-1)
{
}

gcm::CalcNode::CalcNode(int num) : CalcNode(num, 0.0, 0.0, 0.0)
{
}

gcm::CalcNode::CalcNode(int num, gcm::real x, gcm::real y, gcm::real z) : ICalcNode(num, x, y, z)
{
    bodyId = -1;
    memset(values, 0, VALUES_NUMBER * sizeof (gcm::real));
    rho = 0;
    materialId = 0;
    publicFlags.flags = 0;
    privateFlags.mainStressCalculated = false;
    errorFlags.flags = 0;
    // Border condition '1' is 'default' one, since '0' is reserved for 'failsafe' one
    borderConditionId = 1;
    contactConditionId = 0;
    crackDirection[0] = crackDirection[1] = crackDirection[2] = 0.0;
    damageMeasure = 0.0;
}

gcm::CalcNode::CalcNode(const CalcNode& src): ICalcNode(src)
{
    *this = src;
}

CalcNode& gcm::CalcNode::operator=(const CalcNode &src)
{
    number = src.number;

    copy(src.coords, src.coords + 3, coords);
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

gcm::CalcNode::~CalcNode()
{
}

void gcm::CalcNode::clearState()
{
    clearErrorFlags();
    clearMainStresses();
}

void gcm::CalcNode::clearErrorFlags()
{
    errorFlags.flags = 0;
}

void gcm::CalcNode::clearMainStresses()
{
    privateFlags.mainStressCalculated = false;
}

// FIXME get rid of "using namespace std" in header files
gcm::real gcm::CalcNode::getCompression() const
{
    gcm::real compression = 0;
    gcm::real s[3];
    getMainStressComponents(s[0], s[1], s[2]);

    for (int i = 0; i < 3; i++)
        if (s[i] < compression)
            compression = s[i];

    return fabs(compression);
}
// FIXME get rid of "using namespace std" in header files
gcm::real gcm::CalcNode::getTension() const
{
    gcm::real tension = 0;
    gcm::real s[3];
    getMainStressComponents(s[0], s[1], s[2]);

    for (int i = 0; i < 3; i++)
        if (s[i] > tension)
            tension = s[i];

    return tension;
}

// See http://www.toehelp.ru/theory/sopromat/6.html for details
// FIXME get rid of "using namespace std" in header files
gcm::real gcm::CalcNode::getShear() const
{
    gcm::real shear = 0;
    gcm::real s[3];
    gcm::real t[3];
    getMainStressComponents(s[0], s[1], s[2]);

    t[0] = 0.5 * fabs(s[1] - s[0]);
    t[1] = 0.5 * fabs(s[2] - s[0]);
    t[2] = 0.5 * fabs(s[2] - s[1]);

    for (int i = 0; i < 3; i++)
        if (t[i] > shear)
            shear = t[i];

    return shear;
}
// FIXME get rid of "using namespace std" in header files
gcm::real gcm::CalcNode::getDeviator() const
{
    return sqrt(((sxx - syy) * (sxx - syy) + (syy - szz) * (syy - szz) + (sxx - szz) * (sxx - szz)
                + 6 * (sxy * sxy + sxz * sxz + syz * syz)) / 6);
}
// FIXME get rid of "using namespace std" in header files
gcm::real gcm::CalcNode::getPressure() const
{
    gcm::real pressure = -(sxx + syy + szz) / 3;
    return pressure;
}
// FIXME get rid of "using namespace std" in header files
gcm::real gcm::CalcNode::getJ1() const
{
    return sxx + syy + szz;
}
// FIXME get rid of "using namespace std" in header files
gcm::real gcm::CalcNode::getJ2() const
{
    return sxx * syy + sxx * szz + syy * szz - (sxy * sxy + sxz * sxz + syz * syz);
}
// FIXME get rid of "using namespace std" in header files
gcm::real gcm::CalcNode::getJ3() const
{
    return sxx * syy * szz + 2 * sxy * sxz * syz - sxx * syz * syz - syy * sxz * sxz - szz * sxy*sxy;
}

void gcm::CalcNode::getMainStressComponents(gcm::real& s1, gcm::real& s2, gcm::real& s3) const
{
//    if (!privateFlags.mainStressCalculated)
        calcMainStressComponents();

    s1 = mainStresses[0];
    s2 = mainStresses[1];
    s3 = mainStresses[2];
//    if (s1>1000 || s2>1000 || s3>1000) cout <<endl<<"str: "<<s1<<" "<<s2<<" "<<s3<<" ";
}

// See http://www.toehelp.ru/theory/sopromat/6.html
void gcm::CalcNode::calcMainStressComponents() const
{
    real a = -getJ1();
    real b = getJ2();
    real c = -getJ3();
    
    solvePolynomialThirdOrder(a, b, c, mainStresses[0], mainStresses[1], mainStresses[2]);

    privateFlags.mainStressCalculated = true;
}

void gcm::CalcNode::calcMainStressDirectionByComponent(gcm::real s, vector3r& vector) const
{
    gcm::real zero = 0.0000001, a=0,b=0,c=0,u,v,w,n;
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
    gcm::real norm = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
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

bool gcm::CalcNode::rheologyIsValid() const
{
    // FIXME there is no need to check materialId >= 0 since it's uint
    return materialId >= 0 && rho > 0;
}

bool gcm::CalcNode::isInContact() const
{
    return publicFlags.contact;
}

void gcm::CalcNode::setInContact(bool value)
{
    publicFlags.contact = value;
}

bool gcm::CalcNode::isDestroyed() const
{
    return publicFlags.isDestroyed;
}

void gcm::CalcNode::setDestroyed(bool value)
{
    publicFlags.isDestroyed = value;
}

bool gcm::CalcNode::isContactDestroyed() const
{
    return publicFlags.isContactDestroyed;
}

void gcm::CalcNode::setContactDestroyed(bool value)
{
    publicFlags.isContactDestroyed = value;
}

bool gcm::CalcNode::isLocal() const
{
    return isUsed() && privateFlags.local;
}

bool gcm::CalcNode::isRemote() const
{
    return isUsed() && !isLocal();
}

void gcm::CalcNode::setPlacement(bool local)
{
    setUsed(true);
    privateFlags.local = local;
}

bool gcm::CalcNode::isUsed() const
{
    return privateFlags.used;
}

void gcm::CalcNode::setUsed(bool used)
{
    privateFlags.used = used;
}

bool gcm::CalcNode::isFirstOrder() const
{
    return publicFlags.order == 0;
}

bool gcm::CalcNode::isSecondOrder() const
{
    return publicFlags.order == 1;
}

void gcm::CalcNode::setOrder(uchar order)
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

void gcm::CalcNode::setIsBorder(bool border)
{
    privateFlags.border = border;
}

bool gcm::CalcNode::isBorder() const
{
    return privateFlags.border;
}

bool gcm::CalcNode::isInner() const
{
    return !isBorder();
}

void gcm::CalcNode::setXNeighError()
{
    errorFlags.xNeigh = true;
}

void gcm::CalcNode::setYNeighError()
{
    errorFlags.yNeigh = true;
}

void gcm::CalcNode::setZNeighError()
{
    errorFlags.zNeigh = true;
}

void gcm::CalcNode::setNormalError()
{
    errorFlags.outerNormal = true;
}

void gcm::CalcNode::setNeighError(unsigned int axisNum)
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

bool gcm::CalcNode::getCustomFlag(uchar flag) const
{
    switch (flag) {
    case FLAG_1: return publicFlags.flag1;
    case FLAG_2: return publicFlags.flag2;
    case FLAG_3: return publicFlags.flag3;
    case FLAG_4: return publicFlags.flag4;
    default: THROW_INVALID_ARG("Invalid flag specified");
    }
}

void gcm::CalcNode::setCustomFlag(uchar flag, bool value)
{
    switch (flag) {
    case FLAG_1: publicFlags.flag1 = value;
    case FLAG_2: publicFlags.flag2 = value;
    case FLAG_3: publicFlags.flag3 = value;
    case FLAG_4: publicFlags.flag4 = value;
    default: THROW_INVALID_ARG("Invalid flag specified");
    }
}

void gcm::CalcNode::setBorderConditionId(uchar newBorderCondId)
{
    borderConditionId = newBorderCondId;
}

uchar gcm::CalcNode::getBorderConditionId() const
{
    return borderConditionId;
}

void gcm::CalcNode::setContactConditionId(uchar newContactConditionId)
{
    contactConditionId = newContactConditionId;
}

uchar gcm::CalcNode::getContactConditionId() const
{
    return contactConditionId;
}

void gcm::CalcNode::setMaterialId(uchar id)
{
    materialId = id;
    rho = Engine::getInstance().getMaterial(id)->getRho();
}

uchar gcm::CalcNode::getMaterialId() const
{
    return materialId;
}

void gcm::CalcNode::setRheologyMatrix(RheologyMatrixPtr matrix)
{
    assert_true(matrix.get());
    rheologyMatrix = matrix;
}

RheologyMatrixPtr gcm::CalcNode::getRheologyMatrix() const
{
    return rheologyMatrix;
}

MaterialPtr gcm::CalcNode::getMaterial() const
{
    return Engine::getInstance().getMaterial(materialId);
}

void gcm::CalcNode::setRho(gcm::real rho)
{
    this->rho = rho;
}
// FIXME get rid of "using namespace std" in header files
gcm::real gcm::CalcNode::getRho() const
{
    return rho;
}
// FIXME get rid of "using namespace std" in header files
gcm::real gcm::CalcNode::getRho0() const
{
    return Engine::getInstance().getMaterial(materialId)->getRho();
}

void gcm::CalcNode::setDamageMeasure(gcm::real value)
{
    this->damageMeasure = value;
}

gcm::real gcm::CalcNode::getDamageMeasure() const
{
    return damageMeasure;
}

const vector3r& gcm::CalcNode::getCrackDirection() const
{
    return crackDirection;
}

void gcm::CalcNode::createCrack(int direction)
{
    if (crackDirection*crackDirection == 0.0)
        this->calcMainStressDirectionByComponent(mainStresses[direction], crackDirection);
}

void gcm::CalcNode::createCrack(const vector3r& crack)
{
    crackDirection = crack;
}

void gcm::CalcNode::exciseByCrack()
{
    if (crackDirection*crackDirection != 0.0)
        cleanStressByDirection(getCrackDirection());
}
void gcm::CalcNode::cleanStressByDirection(const vector3r& h)
{
    gcm::real s1 = h[0]*(sxx * h[0] + sxy * h[1] + sxz * h[2]) + h[1]*(sxy * h[0] + syy * h[1] + syz * h[2]) + h[2]*(sxz * h[0] + syz * h[1] + szz * h[2]); //TODO
    sxx -= h[0] * h[0] * s1;
    sxy -= h[0] * h[1] * s1;
    sxz -= h[0] * h[2] * s1;
    syy -= h[1] * h[1] * s1;
    syz -= h[1] * h[2] * s1;
    szz -= h[2] * h[2] * s1;
//    cout<<endl<<"vec: "<<h[0]<<" "<<h[1]<<" "<<h[2]<<"  "<<s1;
}

uchar gcm::CalcNode::getPrivateFlags() const
{
    return privateFlags.flags;
}

void gcm::CalcNode::setPrivateFlags(uchar flags)
{
    privateFlags.flags = flags;
}

uchar gcm::CalcNode::getPublicFlags() const
{
    return publicFlags.flags;
}

void gcm::CalcNode::setPublicFlags(uchar flags)
{
    publicFlags.flags = flags;
}

uchar gcm::CalcNode::getErrorFlags() const
{
    return errorFlags.flags;
}

void gcm::CalcNode::setErrorFlags(uchar flags)
{
    errorFlags.flags = flags;
}
