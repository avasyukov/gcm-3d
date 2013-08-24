#include "DummyDispatcher.h"
#include "Body.h"

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
	myNumberOfWorkers =  numberOfWorkers;
	if( numberOfWorkers > 1 && numberOfWorkers < engine->getNumberOfBodies() )
		throw logic_error("You should have either 1 worker OR more workers than bodies. It's crazy stupid internal dispatcher limitation. We are really sorry.");
	
	LOG_DEBUG("Start preparation for " << numberOfWorkers << " workers");
	assert( numberOfWorkers > 0 );
	
	if( outlines != NULL )
		delete[] outlines;
	outlines = new AABB[numberOfWorkers];
	outlinesNum = numberOfWorkers;
	rank = engine->getRank();
	
	if( numberOfWorkers == 1)
	{
		outlines[rank] = *scene;
		myBodyId = "__any";
		LOG_DEBUG("Preparation done");
		return;
	}
	
	int numberOfBodies = engine->getNumberOfBodies();
	LOG_DEBUG("Start preparation for " << numberOfBodies << " bodies");
	workersPerBody = new int[numberOfBodies];
	float totalVolume = 0;
	float* volumes = new float[numberOfBodies];
	for( int i = 0; i < numberOfBodies; i++ )
	{
		volumes[i] = getBodyOutline(engine->getBody(i)->getId()).getVolume();
		totalVolume += volumes[i];
		LOG_DEBUG("Volume[" << i <<"]: " << volumes[i]);
	}
	LOG_DEBUG("Total volume: " << totalVolume);
	float volumePerProcess = totalVolume / numberOfWorkers;
	LOG_DEBUG("Volume per process: " << volumePerProcess);
	
	for( int i = 0; i < numberOfBodies; i++ )
	{
		workersPerBody[i] = (int)(volumes[i]/volumePerProcess);
		if( workersPerBody[i] == 0 )
			workersPerBody[i] = 1;
	}
	
	while( distributionIsOk() != 0 )
	{
		adjustDistribution();
	}
	
	for( int i = 0; i < numberOfBodies; i++ )
		LOG_DEBUG("Workers for body " << i << ": " << workersPerBody[i]);
	
	int workersPassed = 0;
	int bodyNum = 0;
	while( workersPassed + workersPerBody[bodyNum] < rank + 1 )
	{
		workersPassed += workersPerBody[bodyNum];
		bodyNum++;
	}
	myBodyId = engine->getBody(bodyNum)->getId();
	LOG_DEBUG("Scheduled to work with body: " << myBodyId);

	int slicingDirection = getBodySlicingDirection(myBodyId);
	AABB myScene = getBodyOutline(myBodyId);
	float h = ( myScene.max_coords[slicingDirection] - myScene.min_coords[slicingDirection] ) / workersPerBody[bodyNum];
	LOG_DEBUG("Slicing direction: " << slicingDirection << " Slice: " << h);
	
	int i = rank - workersPassed;
	for( int j = 0; j < 3; j++ )
	{
		if( j != slicingDirection )
		{
			outlines[rank].min_coords[j] = myScene.min_coords[j];
			outlines[rank].max_coords[j] = myScene.max_coords[j];
		}
		else
		{
			outlines[rank].min_coords[j] = myScene.min_coords[j] + i * h;
			outlines[rank].max_coords[j] = myScene.min_coords[j] + (i+1) * h;;
		}
	}
	LOG_DEBUG("Preparation done");
	
	delete[] volumes;
	delete[] workersPerBody;
}

int gcm::DummyDispatcher::distributionIsOk()
{
	int totalNum = 0;
	for( int i = 0; i < engine->getNumberOfBodies(); i++ )
		totalNum += workersPerBody[i];
	if( totalNum > myNumberOfWorkers )
		return 1;
	else if( totalNum < myNumberOfWorkers )
		return -1;
	else
		return 0;
}

int gcm::DummyDispatcher::getLeastComputedBody()
{
	int num = 0;
	for( int i = 0; i < engine->getNumberOfBodies(); i++ )
		if( workersPerBody[num] > workersPerBody[i] )
			num = i;
	return num;
}

int gcm::DummyDispatcher::getMostComputedBody()
{
	int num = 0;
	for( int i = 0; i < engine->getNumberOfBodies(); i++ )
		if( workersPerBody[num] < workersPerBody[i] )
			num = i;
	return num;
}

void gcm::DummyDispatcher::adjustDistribution()
{
	int res = distributionIsOk();
	if( res > 0 )
		workersPerBody[ getMostComputedBody() ]--;
	if( res < 0 )
		workersPerBody[ getLeastComputedBody() ]++;
}

bool gcm::DummyDispatcher::isMine(float coords[3], string bodyId) {
	return ( getOwner( coords[0], coords[1], coords[2], bodyId ) == rank );
}

bool gcm::DummyDispatcher::isMine(double coords[3], string bodyId) {
	return ( getOwner( coords[0], coords[1], coords[2], bodyId ) == rank );
}

int gcm::DummyDispatcher::getOwner(float coords[3], string bodyId)
{
	return getOwner( coords[0], coords[1], coords[2], bodyId );
}

int gcm::DummyDispatcher::getOwner(float x, float y, float z, string bodyId)
{
	//LOG_DEBUG("w: " << engine->getNumberOfWorkers() << " bid:" << bodyId << " mbid: " << myBodyId);
	if( engine->getNumberOfWorkers() != 1 && bodyId != myBodyId )
		return -1;
	//LOG_DEBUG("dr: " << dX << " " << dY << " " << dZ);
	for( int i = 0; i < outlinesNum; i++ )
	{
		//LOG_DEBUG("OUTL [" << i << "] " << outlines[i]);
		//LOG_DEBUG("Point: "  << x + dX << " " << y + dY << " " << z + dZ);
		if( outlines[i].isInAABB(x+dX,y+dY,z+dZ) )
			return i;
	}
	return -1;
}

int gcm::DummyDispatcher::getOwner(float coords[3])
{
	return getOwner( coords[0], coords[1], coords[2] );
}

int gcm::DummyDispatcher::getOwner(float x, float y, float z)
{
	//LOG_DEBUG("w: " << engine->getNumberOfWorkers() << " bid:" << bodyId << " mbid: " << myBodyId);
	//if( engine->getNumberOfWorkers() != 1 && bodyId != myBodyId )
	//	return -1;
	//LOG_DEBUG("dr: " << dX << " " << dY << " " << dZ);
	for( int i = 0; i < outlinesNum; i++ )
	{
		//LOG_DEBUG("OUTL [" << i << "] " << outlines[i]);
		//LOG_DEBUG("Point: "  << x + dX << " " << y + dY << " " << z + dZ);
		if( outlines[i].isInAABB(x+dX,y+dY,z+dZ) )
			return i;
	}
	return -1;
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
	LOG_DEBUG("My space shift: [" << dX << "; " << dY << "; " << dZ << "]");
}