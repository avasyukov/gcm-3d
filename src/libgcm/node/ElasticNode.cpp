#include "ElasticNode.h"

gcm::ElasticNode::ElasticNode() {
	la = 0;
	mu = 0;
	rho = 0;
}

gcm::ElasticNode::ElasticNode(int _num) {
	
}

gcm::ElasticNode::ElasticNode(int _num, float _x, float _y, float _z) {
	
}

gcm::ElasticNode::~ElasticNode() {
	
}

gcm::ElasticNode::ElasticNode(const ElasticNode& src) {
	number = src.number;
	memcpy( coords, src.coords, 3*sizeof(float) );
	memcpy( values, src.values, 9*sizeof(float) );
	memcpy( elasticRheologyProperties, src.elasticRheologyProperties, 3*sizeof(float) );
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

ElasticNode& gcm::ElasticNode::operator=(const ElasticNode &src)
{
	number = src.number;
	memcpy( coords, src.coords, 3*sizeof(float) );
	memcpy( values, src.values, 9*sizeof(float) );
	memcpy( elasticRheologyProperties, src.elasticRheologyProperties, 3*sizeof(float) );
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

void gcm::ElasticNode::fastCopy(const ElasticNode& src)
{
	number = src.number;
	memcpy( coords, src.coords, 3*sizeof(float) );
	memcpy( values, src.values, 9*sizeof(float) );
	memcpy( elasticRheologyProperties, src.elasticRheologyProperties, 3*sizeof(float) );
	publicFlags = src.publicFlags;
	privateFlags = src.privateFlags;
	errorFlags = src.errorFlags;
	borderCondId = src.borderCondId;
}