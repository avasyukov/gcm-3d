#include "mesh/Mesh.h"

#include "node/CalcNode.h"

gcm::Mesh::Mesh()
{
	INIT_LOGGER("gcm.Mesh");
	calc = false;
	current_time = 0;
	nodesNumber = 0;
	nodesStorageSize = 0;
	movable = false;
}

gcm::Mesh::~Mesh()
{
	
}

string gcm::Mesh::getType()
{
	return type;
}

void gcm::Mesh::setId(string id)
{
	this->id = id;
}

string gcm::Mesh::getId()
{
	return id;
}

void gcm::Mesh::setCalc(bool calc)
{
	this->calc = calc;
}

bool gcm::Mesh::getCalc()
{
	return calc;
}

void gcm::Mesh::setMovable(bool movable)
{
	this->movable = movable;
}

bool gcm::Mesh::getMovable()
{
	return movable;
}

void gcm::Mesh::setBody(IBody* body)
{
	this->body = body;
}

void gcm::Mesh::setBodyNum(unsigned char id)
{
	CalcNode* node;
	for(int i = 0; i < getNodesNumber(); i++)
	{
		node = getNodeByLocalIndex(i);
		node->bodyId = id;
	}
}

IBody* gcm::Mesh::getBody()
{
	return body;
}

void gcm::Mesh::update_current_time(float time_step)
{
	current_time += time_step;
}

float gcm::Mesh::get_current_time()
{
	return current_time;
}

void gcm::Mesh::snapshot(int number)
{
	SnapshotWriter* sw = Engine::getInstance().getSnapshotWriter(snapshotWriterType);
	sw->dump(this, number);
}

void gcm::Mesh::dump(int number)
{
	SnapshotWriter* sw = Engine::getInstance().getSnapshotWriter(dumpWriterType);
	sw->dump(this, number);
}

AABB gcm::Mesh::getOutline()
{
	return outline;
}

AABB gcm::Mesh::getExpandedOutline()
{
	return expandedOutline;
}

void gcm::Mesh::initNewNodes()
{
	CalcNode* node;
	CalcNode* newNode;
	for(int i = 0; i < getNodesNumber(); i++)
	{
		node = getNodeByLocalIndex(i);
		newNode = getNewNode( node->number );
		memcpy( newNode->coords, node->coords, 3*sizeof(float) );
		memcpy( newNode->values, node->values, GCM_VALUES_SIZE*sizeof(float) );
		newNode->setRho( node->getRho() );
		newNode->setMaterialId( node->getMaterialId() );
		newNode->setContactConditionId(node->getContactConditionId());
		newNode->createCrack(node->getCrackDirection());
	}
}

void gcm::Mesh::preProcess()
{
	LOG_DEBUG("Preprocessing mesh started.");
	initNewNodes();
	createOutline();
	calcMinH();
	preProcessGeometry();
	LOG_DEBUG("Preprocessing mesh done.");
	logMeshStats();
}

void gcm::Mesh::createOutline()
{
	int nodesNumber = getNodesNumber();
	if (nodesNumber > 0)
	{
		LOG_DEBUG("Creating outline");

		CalcNode* node;
		
		// Create outline
		for(int j = 0; j < 3; j++)
		{
			outline.min_coords[j] = numeric_limits<float>::infinity();
			outline.max_coords[j] = - numeric_limits<float>::infinity();
			expandedOutline.min_coords[j] = numeric_limits<float>::infinity();
			expandedOutline.max_coords[j] = - numeric_limits<float>::infinity();
		}

		for(int i = 0; i < getNodesNumber(); i++)
		{
			node = getNodeByLocalIndex(i);
			if( node->isLocal() )
			{
				for(int j = 0; j < 3; j++) {
					if(node->coords[j] > outline.max_coords[j])
						outline.max_coords[j] = node->coords[j];
					if(node->coords[j] < outline.min_coords[j])
						outline.min_coords[j] = node->coords[j];
				}
			}
			for(int j = 0; j < 3; j++) {
				if(node->coords[j] > expandedOutline.max_coords[j])
					expandedOutline.max_coords[j] = node->coords[j];
				if(node->coords[j] < expandedOutline.min_coords[j])
					expandedOutline.min_coords[j] = node->coords[j];
			}
		}
	} else
	{
		LOG_DEBUG ("Mesh is empty, no outline to create");
	}
}

void gcm::Mesh::setInitialState(Area* area, float* values)
{
	CalcNode* node;
	for(int i = 0; i < getNodesNumber(); i++)
	{
		node = getNodeByLocalIndex(i);
		if( area->isInArea( node ) )
			for( int k = 0; k < 9; k++ )
				node->values[k] = values[k];
	}
}

void gcm::Mesh::setRheology(unsigned char matId) {
	CalcNode* node;
	for(int i = 0; i < getNodesNumber(); i++)
	{
		node = getNodeByLocalIndex(i);
		node->setMaterialId( matId );
		node->initRheology();
	}
}

void gcm::Mesh::setRheology(unsigned char matId, Area* area) {
	CalcNode* node;
	for(int i = 0; i < getNodesNumber(); i++)
	{
		node = getNodeByLocalIndex(i);
		if( area->isInArea(node) )
		{
			node->setMaterialId( matId );
			node->initRheology();
		}
	}
}

void gcm::Mesh::transfer(float x, float y, float z)
{
	CalcNode* node;
	for(int i = 0; i < getNodesNumber(); i++)
	{
		node = getNodeByLocalIndex(i);
		node->coords[0] += x;
		node->coords[1] += y;
		node->coords[2] += z;
	}
	if( !isinf(outline.minX) )
	{
		outline.transfer(x, y, z);
	}
	if( !isinf(expandedOutline.minX) )
	{
		expandedOutline.transfer(x, y, z);
	}
	if( !isinf(syncedArea.minX) )
	{
		syncedArea.transfer(x, y, z);
	}
	if( !isinf(areaOfInterest.minX) )
	{
		areaOfInterest.transfer(x, y, z);
	}
	
	// TODO@avasyukov - think about additional checks
	body->getEngine()->transferScene(x, y, z);
}

void gcm::Mesh::applyRheology(RheologyCalculator* rc)
{
	CalcNode* node;
	for(int i = 0; i < getNodesNumber(); i++)
	{
		node = getNodeByLocalIndex(i);
		if( node->isLocal() )
			rc->doCalc(node, node);
	}
}

void gcm::Mesh::clearNodesState()
{
	CalcNode* node;
	for(int i = 0; i < getNodesNumber(); i++)
	{
		node = getNodeByLocalIndex(i);
		if( node->isLocal() )
			node->clearState();
	}
};

void gcm::Mesh::clearContactState()
{
	CalcNode* node;
	for(int i = 0; i < getNodesNumber(); i++)
	{
		node = getNodeByLocalIndex(i);
		if( node->isLocal() )
			node->setContactType(Free);
	}
}

void gcm::Mesh::processCrackState()
{
	CalcNode* node;
	for(int i = 0; i < getNodesNumber(); i++)
	{
		node = getNodeByLocalIndex(i);
		if( node->isLocal() )
		{
			float m_s[3];
			node->getMainStressComponents(m_s[0], m_s[1], m_s[2]);
			int i_ms=0; if (m_s[1]>m_s[i_ms]) i_ms=1; if (m_s[2]>m_s[i_ms]) i_ms = 2;
			if (m_s[i_ms] > node->getCrackThreshold())
			{
				node->createCrack(i_ms);
				LOG_TRACE("New crack detected at node " << *node);
			}
		}
	}
}

void gcm::Mesh::processCrackResponse()
{
	CalcNode* node;
	for(int i = 0; i < getNodesNumber(); i++)
	{
		node = getNodeByLocalIndex(i);
		if( node->isLocal() )
		{
			float *m_s = node->getCrackDirection();
			if (scalarProduct(m_s,m_s)>0.5)
			{
				node->cleanStressByDirection(m_s);
				LOG_TRACE("Existing crack found at node " << *node);
			}
		}
	}
}

void gcm::Mesh::processStressState()
{
	CalcNode* node;
	for(int i = 0; i < getNodesNumber(); i++)
	{
		node = getNodeByLocalIndex(i);
		if( node->isLocal() )
			node->calcMainStressComponents();
	}
}

void gcm::Mesh::moveCoords(float tau)
{
	LOG_DEBUG("Moving mesh coords");
	CalcNode* node;
	CalcNode* newNode;
	for(int i = 0; i < getNodesNumber(); i++)
	{
		node = getNodeByLocalIndex(i);
		if( node->isLocal() && node->isFirstOrder() )
		{
			newNode = getNewNode( node->number );
			for(int j = 0; j < 3; j++)
			{
				// Move node
				node->coords[j] += node->values[j]*tau;
				newNode->coords[j] = node->coords[j];
				// Move mesh outline if necessary
				if(node->coords[j] > outline.max_coords[j])
					outline.max_coords[j] = node->coords[j];
				if(node->coords[j] < outline.min_coords[j])
					outline.min_coords[j] = node->coords[j];
			}
		}
	}
	calcMinH();
};

float gcm::Mesh::getMaxLambda()
{
	NumericalMethod *method = body->getEngine()->getNumericalMethod(numericalMethodType);
	assert(method != NULL);
	CalcNode* node;
	float maxLambda = 0;
	for(int i = 0; i < getNodesNumber(); i++)
	{
		node = getNodeByLocalIndex(i);
		float lambda = method->getMaxLambda( node );
		if( lambda > maxLambda )
			maxLambda = lambda;
	}
	return maxLambda;
}

float gcm::Mesh::getMaxPossibleTimeStep()
{
	NumericalMethod *method = body->getEngine()->getNumericalMethod(numericalMethodType);
	CalcNode* node;
	float maxLambda = 0;
	for(int i = 0; i < getNodesNumber(); i++)
	{
		node = getNodeByLocalIndex(i);
		float lambda = method->getMaxLambda( node );
		if( lambda > maxLambda )
			maxLambda = lambda;
	}
	LOG_DEBUG( "Min H over mesh is " << getMinH() );
	LOG_DEBUG( "Max lambda over mesh is " << maxLambda );
	LOG_DEBUG( "Courant time step is " << getMinH() / maxLambda );
	return getMinH() / maxLambda;
}

int gcm::Mesh::getNodesNumber()
{
	return nodesNumber;
}

int gcm::Mesh::getNumberOfLocalNodes()
{
	int num = 0;
	CalcNode* node;
	for(int i = 0; i < getNodesNumber(); i++)
	{
		node = getNodeByLocalIndex(i);
		if( getNode(i)->isLocal() )
			num++;
	}
	return num;
}

void gcm::Mesh::createNodes(int number) {
	LOG_DEBUG("Creating nodes storage, size: " << (int)(number*STORAGE_OVERCOMMIT_RATIO));
	nodes.resize((int)(number*STORAGE_OVERCOMMIT_RATIO));
	new_nodes.resize((int)(number*STORAGE_OVERCOMMIT_RATIO));
	nodesStorageSize = number*STORAGE_OVERCOMMIT_RATIO;
}

CalcNode* gcm::Mesh::getNode(int index)
{
	assert( index >= 0 );
	unordered_map<int, int>::const_iterator itr;
	itr = nodesMap.find(index);
	return ( itr != nodesMap.end() ? &nodes[itr->second] : NULL );
}

CalcNode* gcm::Mesh::getNewNode(int index) {
	assert( index >= 0 );
	unordered_map<int, int>::const_iterator itr;
	itr = nodesMap.find(index);
	return ( itr != nodesMap.end() ? &new_nodes[itr->second] : NULL );
}

CalcNode* gcm::Mesh::getNodeByLocalIndex(int index) {
	assert( index >= 0 && index < nodes.size() );
	return &nodes[index];
}

int gcm::Mesh::getNodeLocalIndex(int index) {
	assert( index >= 0 );
	unordered_map<int, int>::const_iterator itr;
	itr = nodesMap.find(index);
	return ( itr != nodesMap.end() ? itr->second : -1 );
}

void gcm::Mesh::addNode(CalcNode* node) {
	if( nodesNumber == nodesStorageSize )
		createNodes((nodesStorageSize+1)*STORAGE_ONDEMAND_GROW_RATE);
	assert( nodesNumber < nodesStorageSize );
	nodes[nodesNumber] = *node;
	nodesMap[node->number] = nodesNumber;
	nodesNumber++;
}

void gcm::Mesh::defaultNextPartStep(float tau, int stage)
{
	LOG_DEBUG("Nodes: " << nodesNumber);
	
	if( stage == 0 )
	{
		LOG_DEBUG("Clear error flags on all nodes");
		clearNodesState();
	}
	
	CalcNode* node;
	NumericalMethod *method = body->getEngine()->getNumericalMethod(numericalMethodType);
	method->setSpaceOrder(numericalMethodOrder);

	if( ! syncedArea.includes( &areaOfInterest ) )
	{
		LOG_ERROR("Area of interest: " << areaOfInterest);
		LOG_ERROR("Synced area: " << syncedArea);
		assert( syncedArea.includes( &areaOfInterest ) );
	}
	
	// Border nodes
	LOG_DEBUG("Processing border nodes");
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		node = getNode(i);
		if( node->isLocal() && node->isBorder() )
				method->doNextPartStep( node, getNewNode(i), tau, stage, this );
	}

	// Inner nodes
	LOG_DEBUG("Processing inner nodes");
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		node = getNode(i);
		if( node->isLocal() && node->isInner() )
				method->doNextPartStep( node, getNewNode(i), tau, stage, this );
	}

	// Copy values	
	LOG_DEBUG("Copying values");
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		node = getNode(i);
		if( node->isLocal() )
			memcpy( node->values, getNewNode(i)->values, GCM_VALUES_SIZE * sizeof(float) );
	}
}
