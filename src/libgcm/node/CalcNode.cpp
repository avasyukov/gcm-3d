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
	addOwner( GCM );
	elements = new vector<int>;
	border_elements = new vector<int>;
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
	elements = new vector<int>;
	border_elements = new vector<int>;
	for( unsigned int i = 0; i < src.elements->size(); i++ )
		elements->push_back( src.elements->at(i) );
	for( unsigned int i = 0; i < src.border_elements->size(); i++ )
		border_elements->push_back( src.border_elements->at(i) );
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
	elements->clear();
	border_elements->clear();
	for( unsigned int i = 0; i < src.elements->size(); i++ )
		elements->push_back( src.elements->at(i) );
	for( unsigned int i = 0; i < src.border_elements->size(); i++ )
		border_elements->push_back( src.border_elements->at(i) );
	return *this;
}

gcm::CalcNode::~CalcNode()
{
	elements->clear();
	delete elements;
	border_elements->clear();
	delete border_elements;
}

void gcm::CalcNode::clearErrorFlags()
{
	errorFlags = 0;
}

float gcm::CalcNode::getCompression()
{
	float compression = 0;
	// FIXME - we need main tensor components, not diagonal components
	if( values[3] < compression )
		compression = values[3];
	if( values[6] < compression )
		compression = values[6];
	if( values[8] < compression )
		compression = values[8];
	return fabs(compression);
}

float gcm::CalcNode::getTension()
{
	float tension = 0;
	// FIXME - we need main tensor components, not diagonal components
	if( values[3] > tension )
		tension = values[3];
	if( values[6] > tension )
		tension = values[6];
	if( values[8] > tension )
		tension = values[8];
	return tension;
}

float gcm::CalcNode::getShear()
{
	float shear = 0;
	// FIXME - we need main tensor components, not diagonal components
	if( fabs(values[4]) > shear )
		shear = fabs(values[4]);
	if( fabs(values[5]) > shear )
		shear = fabs(values[5]);
	if( fabs(values[7]) > shear )
		shear = fabs(values[7]);
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
void gcm::CalcNode::calcMainStressComponents(float& s1, float& s2, float& s3)
{
	float a = - getJ1();
	float b = getJ2();
	float c = - getJ3();
	
	float p = b - a * a / 3.0;
	float q = 2.0 * a * a * a / 27.0 - a * b / 3.0 + c;
	float A = sqrt(- 4.0 * p / 3.0);
	float c3phi = - 4.0 * q / (A * A * A);
	float phi = acos(c3phi) / 3.0;
	
	s1 = A * cos(phi) - a / 3.0;
	s2 = A * cos(phi + 2 * M_PI / 3.0) - a / 3.0;
	s3 = A * cos(phi - 2 * M_PI / 3.0) - a / 3.0;
}