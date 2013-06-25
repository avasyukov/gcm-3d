#include "DummyDispatcher.h"

gcm::DummyDispatcher::DummyDispatcher() {
	INIT_LOGGER("gcm.Dispatcher");
	engine = NULL;
	outlines = NULL;
	outlinesNum = -1;
}

gcm::DummyDispatcher::~DummyDispatcher() {
	if( outlines != NULL )
		delete[] outlines;
}

void gcm::DummyDispatcher::setEngine(IEngine* e) {
	engine = e;
}

void gcm::DummyDispatcher::prepare(int numberOfWorkers, AABB* scene)
{
	LOG_DEBUG("Start preparation for " << numberOfWorkers << " workers");
	assert( numberOfWorkers > 0 );
	if( outlines != NULL )
		delete[] outlines;
	outlines = new AABB[numberOfWorkers];
	outlinesNum = numberOfWorkers;
	rank = engine->getRank();
	
	float h = ( scene->maxZ - scene->minZ ) / numberOfWorkers;
	LOG_DEBUG("Slice: " << h);
	
	//for( int i = 0; i < numberOfWorkers; i++ )
	//{
	int i = rank;
		outlines[i].minX = scene->minX;
		outlines[i].minY = scene->minY;
		outlines[i].minZ = scene->minZ + i * h;
		outlines[i].maxX = scene->maxX;
		outlines[i].maxY = scene->maxY;
		outlines[i].maxZ = scene->minZ + (i+1) * h;
	//}
	LOG_DEBUG("Preparation done");
}

bool gcm::DummyDispatcher::isMine(float coords[3]) {
	return ( getOwner( coords[0], coords[1], coords[2] ) == rank );
}

bool gcm::DummyDispatcher::isMine(double coords[3]) {
	return ( getOwner( coords[0], coords[1], coords[2] ) == rank );
}

int gcm::DummyDispatcher::getOwner(float coords[3])
{
	return getOwner( coords[0], coords[1], coords[2] );
}

int gcm::DummyDispatcher::getOwner(float x, float y, float z)
{
	float h = (outlines[ outlinesNum - 1 ].maxZ - outlines[0].minZ) / outlinesNum;
	int ind = (int)( (z - outlines[0].minZ) / h );
	if( ind == outlinesNum )
		ind--;
	if( ind < 0 || ind >= outlinesNum )
		return -1;
	assert( ind >= 0 && ind < outlinesNum );
	assert( outlines[ind].isInAABB(x,y,z) );
	return ind;
}

AABB* gcm::DummyDispatcher::getOutline(int index)
{
	assert( index >= 0 && index < outlinesNum );
	return outlines + index;
}

void gcm::DummyDispatcher::printZones()
{
	for( int i = 0; i < outlinesNum; i++ )
		LOG_DEBUG("Zone " << i << ": " << outlines[i]);
}