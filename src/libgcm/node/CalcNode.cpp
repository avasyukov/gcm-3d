#include "node/CalcNode.h"

gcm::CalcNode::CalcNode() : CalcNode(-1)
{
}

gcm::CalcNode::CalcNode(int num) : CalcNode(num, 0.0, 0.0, 0.0)
{
}

gcm::CalcNode::CalcNode(int num, gcm_real x, gcm_real y, gcm_real z) : ICalcNode(num, x, y, z)
{
    bodyId = -1;
    memset(values, 0, GCM_VALUES_SIZE * sizeof (gcm_real));
    rho = 0;
    materialId = 0;
    publicFlags.flags = 0;
    privateFlags.mainStressCalculated = false;
    errorFlags.flags = 0;
    // Border condition '1' is 'default' one, since '0' is reserved for 'failsafe' one
    borderConditionId = 1;
    contactConditionId = 0;
    crackDirection[0] = crackDirection[1] = crackDirection[2] = 0.0;
}

gcm::CalcNode::CalcNode(const CalcNode& src): ICalcNode(src)
{
    *this = src;
}

CalcNode& gcm::CalcNode::operator=(const CalcNode &src)
{
    number = src.number;

    copy(src.coords, src.coords + 3, coords);
    copy(src.values, src.values + GCM_VALUES_SIZE, values);
    copy(src.crackDirection, src.crackDirection + 3, crackDirection);

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

gcm_real gcm::CalcNode::getCompression() const
{
    gcm_real compression = 0;
    gcm_real s[3];
    getMainStressComponents(s[0], s[1], s[2]);

    for (int i = 0; i < 3; i++)
        if (s[i] < compression)
            compression = s[i];

    return fabs(compression);
}

gcm_real gcm::CalcNode::getTension() const
{
    gcm_real tension = 0;
    gcm_real s[3];
    getMainStressComponents(s[0], s[1], s[2]);

    for (int i = 0; i < 3; i++)
        if (s[i] > tension)
            tension = s[i];

    return tension;
}

// See http://www.toehelp.ru/theory/sopromat/6.html for details

gcm_real gcm::CalcNode::getShear() const
{
    gcm_real shear = 0;
    gcm_real s[3];
    gcm_real t[3];
    getMainStressComponents(s[0], s[1], s[2]);

    t[0] = 0.5 * fabs(s[1] - s[0]);
    t[1] = 0.5 * fabs(s[2] - s[0]);
    t[2] = 0.5 * fabs(s[2] - s[1]);

    for (int i = 0; i < 3; i++)
        if (t[i] > shear)
            shear = t[i];

    return shear;
}

gcm_real gcm::CalcNode::getDeviator() const
{
    return sqrt(((sxx - syy) * (sxx - syy) + (syy - szz) * (syy - szz) + (sxx - szz) * (sxx - szz)
                + 6 * (sxy * sxy + sxz * sxz + syz * syz)) / 6);
}

gcm_real gcm::CalcNode::getPressure() const
{
    gcm_real pressure = -(sxx + syy + szz) / 3;
    return pressure;
}

gcm_real gcm::CalcNode::getJ1() const
{
    return sxx + syy + szz;
}

gcm_real gcm::CalcNode::getJ2() const
{
    return sxx * syy + sxx * szz + syy * szz - (sxy * sxy + sxz * sxz + syz * syz);
}

gcm_real gcm::CalcNode::getJ3() const
{
    return sxx * syy * szz + 2 * sxy * sxz * syz - sxx * syz * syz - syy * sxz * sxz - szz * sxy*sxy;
}

void gcm::CalcNode::getMainStressComponents(gcm_real& s1, gcm_real& s2, gcm_real& s3) const
{
    if (!privateFlags.mainStressCalculated)
        calcMainStressComponents();

    s1 = mainStresses[0];
    s2 = mainStresses[1];
    s3 = mainStresses[2];
}

// See http://www.toehelp.ru/theory/sopromat/6.html
//  and http://ru.wikipedia.org/wiki/Тригонометрическая_формула_Виета for algo

void gcm::CalcNode::calcMainStressComponents() const
{
    gcm_real a = -getJ1();
    gcm_real b = getJ2();
    gcm_real c = -getJ3();

    gcm_real p = b - a * a / 3.0;
    gcm_real q = 2.0 * a * a * a / 27.0 - a * b / 3.0 + c;
    gcm_real A = sqrt(-4.0 * p / 3.0);
    gcm_real c3phi = -4.0 * q / (A * A * A);
    gcm_real phi = acos(c3phi) / 3.0;

    mainStresses[0] = A * cos(phi) - a / 3.0;
    mainStresses[1] = A * cos(phi + 2 * M_PI / 3.0) - a / 3.0;
    mainStresses[2] = A * cos(phi - 2 * M_PI / 3.0) - a / 3.0;

    privateFlags.mainStressCalculated = true;
}

void gcm::CalcNode::calcMainStressDirectionByComponent(gcm_real s, vector3& vector) const
{
    if ((sxy * sxy - (sxx - s)*(syy - s))*(sxy * (szz - s) - sxz * syz)-(sxy * sxz - (sxx - s) * syz)*(sxy * syz - sxz * (syy - s)) == 0)
        vector[2] = 1;
    else
        vector[2] = 0;

    if (sxy * sxy - (sxx - s)*(syy - s) != 0)
        vector[1] = vector[2]*(-sxy * sxz + (sxx - s) * sxz) / (sxy * sxy - (sxx - s)*(syy - s));
    else if (sxy * syz - sxz * (syy - s) != 0)
        vector[1] = vector[2]*(sxz * syz - sxy * (szz - s)) / (sxy * syz - sxz * (syy - s));
    else
        vector[1] = 1;

    if (sxx - s != 0)
        vector[0] = -vector[1] * sxy / (sxx - s) - vector[2] * sxz / (sxx - s);
    else if (sxy != 0)
        vector[0] = -vector[1]*(syy - s) / sxy - vector[2] * syz / sxy;
    else if (sxz != 0)
        vector[0] = -vector[1] * syz / sxz - vector[2]*(szz - s) / sxz;
    else
        vector[0] = 1;

    gcm_real norm = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
    if (norm > 0.0) {
        vector[0] /= norm;
        vector[1] /= norm;
        vector[2] /= norm;
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
    case FLAG_5: return publicFlags.flag5;
    case FLAG_6: return publicFlags.flag6;
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
    case FLAG_5: publicFlags.flag5 = value;
    case FLAG_6: publicFlags.flag6 = value;
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

RheologyMatrix3D& gcm::CalcNode::getRheologyMatrix() const
{
    return getMaterial()->getRheologyMatrix();
}

Material* gcm::CalcNode::getMaterial() const
{
    return Engine::getInstance().getMaterial(materialId);
}

void gcm::CalcNode::setRho(gcm_real rho)
{
    this->rho = rho;
}

gcm_real gcm::CalcNode::getRho() const
{
    return rho;
}

gcm_real gcm::CalcNode::getRho0() const
{
    return Engine::getInstance().getMaterial(materialId)->getRho();
}

const vector3& gcm::CalcNode::getCrackDirection() const
{
    return crackDirection;
}

void gcm::CalcNode::createCrack(int direction)
{
    if (scalarProduct(crackDirection, crackDirection) == 0.0)
        this->calcMainStressDirectionByComponent(mainStresses[direction], crackDirection);
}

void gcm::CalcNode::createCrack(const vector3& crack)
{
    copy(crack, crack + 3, crackDirection);
}

void gcm::CalcNode::cleanStressByDirection(const vector3& h)
{
    gcm_real s1 = h[0]*(sxx * h[0] + sxy * h[1] + sxz * h[2]) + h[1]*(sxy * h[0] + syy * h[1] + syz * h[2]) + h[2]*(sxz * h[0] + syz * h[1] + szz * h[2]); //TODO
    sxx -= h[0] * h[0] * s1;
    sxy -= h[0] * h[1] * s1;
    sxz -= h[0] * h[2] * s1;
    syy -= h[1] * h[1] * s1;
    syz -= h[1] * h[2] * s1;
    szz -= h[2] * h[2] * s1;
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