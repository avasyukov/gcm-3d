#include "CalcNode.h"

gcm::CalcNode::CalcNode()
{
	bodyId = -1;
	memset(values, 0, 9*sizeof(float));
	rho = 0;
	materialId = -1;
	publicFlags = 0;
	privateFlags = 0;
	errorFlags = 0;
	// Border condition '1' is 'default' one, since '0' is reserved for 'failsafe' one
	borderCondId = 1;
	contactCondId = 0;
	addOwner( GCM );
	elements = new vector<int>;
	border_elements = new vector<int>;
	crackDirection[0] = crackDirection[1] =	crackDirection[2] = 0.0;
}

gcm::CalcNode::CalcNode(int _num) {
	
}

gcm::CalcNode::CalcNode(int _num, float _x, float _y, float _z) {
	
}

gcm::CalcNode::CalcNode(const CalcNode& src) {
	number = src.number;
	memcpy( coords, src.coords, 3*sizeof(float) );
	memcpy( values, src.values, 9*sizeof(float) );
	bodyId = src.bodyId;
	rho = src.rho;
	materialId = src.materialId;
	contactNodeNum = src.contactNodeNum;
	contactDirection = src.contactDirection;
	publicFlags = src.publicFlags;
	privateFlags = src.privateFlags;
	errorFlags = src.errorFlags;
	borderCondId = src.borderCondId;
	contactCondId = src.contactCondId;
	elements = new vector<int>;
	border_elements = new vector<int>;
	for( unsigned int i = 0; i < src.elements->size(); i++ )
		elements->push_back( src.elements->at(i) );
	for( unsigned int i = 0; i < src.border_elements->size(); i++ )
		border_elements->push_back( src.border_elements->at(i) );
	memcpy( crackDirection, src.crackDirection, 3*sizeof(float));
}

CalcNode& gcm::CalcNode::operator=(const CalcNode &src)
{
	number = src.number;
	memcpy( coords, src.coords, 3*sizeof(float) );
	memcpy( values, src.values, 9*sizeof(float) );
	bodyId = src.bodyId;
	rho = src.rho;
	materialId = src.materialId;
	contactNodeNum = src.contactNodeNum;
	contactDirection = src.contactDirection;
	publicFlags = src.publicFlags;
	privateFlags = src.privateFlags;
	errorFlags = src.errorFlags;
	borderCondId = src.borderCondId;
	contactCondId = src.contactCondId;
	elements->clear();
	border_elements->clear();
	for( unsigned int i = 0; i < src.elements->size(); i++ )
		elements->push_back( src.elements->at(i) );
	for( unsigned int i = 0; i < src.border_elements->size(); i++ )
		border_elements->push_back( src.border_elements->at(i) );
	memcpy( crackDirection, src.crackDirection, 3*sizeof(float));
	return *this;
}

gcm::CalcNode::~CalcNode()
{
	elements->clear();
	delete elements;
	border_elements->clear();
	delete border_elements;
}

void gcm::CalcNode::clearState()
{
	clearErrorFlags();
	clearMainStresses();
}

void gcm::CalcNode::clearErrorFlags()
{
	errorFlags = 0;
}

void gcm::CalcNode::clearMainStresses()
{
	setMainStressCalculated( false );
}

float gcm::CalcNode::getCompression()
{
	float compression = 0;
	float s[3];
	getMainStressComponents(s[1], s[2], s[3]);
	
	for( int i = 0; i < 3; i++ )
		if( s[i] < compression )
			compression = s[i];
	
	return fabs(compression);
}

float gcm::CalcNode::getTension()
{
	float tension = 0;
	float s[3];
	getMainStressComponents(s[1], s[2], s[3]);
	
	for( int i = 0; i < 3; i++ )
		if( s[i] > tension )
			tension = s[i];
	
	return tension;
}

// See http://www.toehelp.ru/theory/sopromat/6.html for details
float gcm::CalcNode::getShear()
{
	float shear = 0;
	float s[3];
	float t[3];
	getMainStressComponents(s[1], s[2], s[3]);
	
	t[0] = 0.5 * fabs(s[1] - s[0]);
	t[1] = 0.5 * fabs(s[2] - s[0]);
	t[2] = 0.5 * fabs(s[2] - s[1]);
	
	for( int i = 0; i < 3; i++ )
		if( t[i] > shear )
			shear = t[i];
	
	return shear;
}

float gcm::CalcNode::getDeviator()
{
	return sqrt( ( (sxx - syy) * (sxx - syy) + (syy - szz) * (syy - szz) + (sxx - szz) * (sxx - szz)
					+ 6 * ( sxy * sxy + sxz * sxz + syz * syz) ) / 6 );
}

float gcm::CalcNode::getPressure()
{
	float pressure = - ( sxx + syy + szz ) / 3;
	return pressure;
}

float gcm::CalcNode::getJ1()
{
	return sxx + syy + szz;
}

float gcm::CalcNode::getJ2()
{
	return sxx*syy + sxx*szz + syy*szz - ( sxy*sxy + sxz*sxz + syz*syz );
}

float gcm::CalcNode::getJ3()
{
	return sxx*syy*szz + 2*sxy*sxz*syz - sxx*syz*syz - syy*sxz*sxz - szz*sxy*sxy;
}

// See http://www.toehelp.ru/theory/sopromat/6.html 
//	and http://ru.wikipedia.org/wiki/Тригонометрическая_формула_Виета for algo
void gcm::CalcNode::calcMainStressComponents()
{
	float a = - getJ1();
	float b = getJ2();
	float c = - getJ3();
	
	float p = b - a * a / 3.0;
	float q = 2.0 * a * a * a / 27.0 - a * b / 3.0 + c;
	float A = sqrt(- 4.0 * p / 3.0);
	float c3phi = - 4.0 * q / (A * A * A);
	float phi = acos(c3phi) / 3.0;
	
	mainStresses[0] = A * cos(phi) - a / 3.0;
	mainStresses[1] = A * cos(phi + 2 * M_PI / 3.0) - a / 3.0;
	mainStresses[2] = A * cos(phi - 2 * M_PI / 3.0) - a / 3.0;
	setMainStressCalculated( true );
}

void gcm::CalcNode::calcMainStressDirectionByComponent(float s, float* vector)
{
	if ( (sxy*sxy-(sxx-s)*(syy-s))*(sxy*(szz-s)-sxz*syz)-(sxy*sxz-(sxx-s)*syz)*(sxy*syz-sxz*(syy-s)) == 0)
		vector[2]=1;
	else	vector[2]=0;

	if (sxy*sxy-(sxx-s)*(syy-s) != 0)
		vector[1] = vector[2]*(-sxy*sxz+(sxx-s)*sxz)/(sxy*sxy-(sxx-s)*(syy-s));
	else if (sxy*syz-sxz*(syy-s) != 0)
		vector[1] = vector[2]*(sxz*syz-sxy*(szz-s))/(sxy*syz-sxz*(syy-s));
	else 	vector[1] = 1;

	if (sxx-s != 0)
		vector[0] = -vector[1]*sxy/(sxx-s)-vector[2]*sxz/(sxx-s);
	else if (sxy != 0) 
		vector[0] = -vector[1]*(syy-s)/sxy-vector[2]*syz/sxy;
	else if (sxz != 0) 
		vector[0] = -vector[1]*syz/sxz-vector[2]*(szz-s)/sxz;
	else 	vector[0] = 1;

	float norm = sqrt(vector[0]*vector[0]+vector[1]*vector[1]+vector[2]*vector[2]);
	if (norm > 0.0)
	{
		vector[0]/=norm;
		vector[1]/=norm;
		vector[2]/=norm;
	}
}

void gcm::CalcNode::createCrack(int direction)
{
	if (scalarProduct(crackDirection,crackDirection) < 0.1)
		this->calcMainStressDirectionByComponent(mainStresses[direction], crackDirection);
}

void gcm::CalcNode::createCrack(float* vector)
{
	memcpy( crackDirection, vector, 3*sizeof(float) );
}


void gcm::CalcNode::getMainStressComponents(float& s1, float& s2, float& s3)
{
	if( ! isMainStressCalculated() )
		calcMainStressComponents();
	
	s1 = mainStresses[0];
	s2 = mainStresses[1];
	s3 = mainStresses[2];
}
