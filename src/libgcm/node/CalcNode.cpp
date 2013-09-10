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
	return sqrt( ( (values[3] - values[6]) * (values[3] - values[6]) 
					+ (values[6] - values[8]) * (values[6] - values[8])
					+ (values[3] - values[8]) * (values[3] - values[8])
					+ 6 * ( (values[4]) * (values[4]) + (values[5]) * (values[5])
							+ (values[7]) * (values[7])) ) / 6 );
}

float gcm::CalcNode::getPressure()
{
	float pressure = -(values[3]+values[6]+values[8])/3;
	return pressure;
}
