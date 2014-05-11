#include "libgcm/mesh/Mesh.hpp"

#include "libgcm/node/CalcNode.hpp"

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
    for(int i = 0; i < getNodesNumber(); i++)
    {
        CalcNode& node = getNodeByLocalIndex(i);
        node.bodyId = id;
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

// FIXME
// should it be const reference instead of copy?
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
    for(int i = 0; i < getNodesNumber(); i++)
    {
        CalcNode& node = getNodeByLocalIndex(i);
        CalcNode& newNode = getNewNode( node.number );
        copy(node.coords, node.coords + 3, newNode.coords);
        copy(node.values, node.values + VALUES_NUMBER, newNode.values);
        newNode.setRho(node.getRho());
        newNode.setMaterialId(node.getMaterialId());
        newNode.setContactConditionId(node.getContactConditionId());
        newNode.createCrack(node.getCrackDirection());
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
            CalcNode& node = getNodeByLocalIndex(i);
            if( node.isLocal() )
            {
                for(int j = 0; j < 3; j++) {
                    if(node.coords[j] > outline.max_coords[j])
                        outline.max_coords[j] = node.coords[j];
                    if(node.coords[j] < outline.min_coords[j])
                        outline.min_coords[j] = node.coords[j];
                }
            }
            for(int j = 0; j < 3; j++) {
                if(node.coords[j] > expandedOutline.max_coords[j])
                    expandedOutline.max_coords[j] = node.coords[j];
                if(node.coords[j] < expandedOutline.min_coords[j])
                    expandedOutline.min_coords[j] = node.coords[j];
            }
        }
    } else
    {
        LOG_DEBUG ("Mesh is empty, no outline to create");
    }
}

void gcm::Mesh::setInitialState(Area* area, float* values)
{
    for(int i = 0; i < getNodesNumber(); i++)
    {
        CalcNode& node = getNodeByLocalIndex(i);
        if( area->isInArea( node ) )
            for( int k = 0; k < 9; k++ )
                node.values[k] = values[k];
    }
}

void gcm::Mesh::setBorderCondition(Area* area, unsigned int num)
{
    for(int i = 0; i < getNodesNumber(); i++)
    {
        CalcNode& node = getNodeByLocalIndex(i);
        if( area->isInArea( node ) )
            node.setBorderConditionId(num);
    }
}

void gcm::Mesh::setContactCondition(Area* area, unsigned int num)
{
    for(int i = 0; i < getNodesNumber(); i++)
    {
        CalcNode& node = getNodeByLocalIndex(i);
        if( area->isInArea( node ) )
            node.setContactConditionId(num);
    }
}

void gcm::Mesh::setRheology(unsigned char matId) {
    for(int i = 0; i < getNodesNumber(); i++)
    {
        CalcNode& node = getNodeByLocalIndex(i);
        node.setMaterialId( matId );
    }
}

void gcm::Mesh::setRheology(unsigned char matId, Area* area) {
    for(int i = 0; i < getNodesNumber(); i++)
    {
        CalcNode& node = getNodeByLocalIndex(i);
        if( area->isInArea(node) )
        {
            node.setMaterialId( matId );
        }
    }
}

void gcm::Mesh::transfer(float x, float y, float z)
{
    for(int i = 0; i < getNodesNumber(); i++)
    {
        CalcNode& node = getNodeByLocalIndex(i);
        node.coords[0] += x;
        node.coords[1] += y;
        node.coords[2] += z;
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

void gcm::Mesh::scale(float x0, float y0, float z0, 
		float scaleX, float scaleY, float scaleZ)
{
    for(int i = 0; i < getNodesNumber(); i++)
    {
        CalcNode& node = getNodeByLocalIndex(i);
        node.coords[0] = (node.coords[0] - x0)*scaleX + x0;
        node.coords[1] = (node.coords[1] - y0)*scaleY + y0;
        node.coords[2] = (node.coords[2] - z0)*scaleZ + z0;
    }
    if( !isinf(outline.minX) )
    {
        outline.scale(x0, y0, z0, scaleX, scaleY, scaleZ);
    }
    if( !isinf(expandedOutline.minX) )
    {
        expandedOutline.scale(x0, y0, z0, scaleX, scaleY, scaleZ);
    }
    if( !isinf(syncedArea.minX) )
    {
        syncedArea.scale(x0, y0, z0, scaleX, scaleY, scaleZ);
    }
    if( !isinf(areaOfInterest.minX) )
    {
        areaOfInterest.scale(x0, y0, z0, scaleX, scaleY, scaleZ);
    }
    body->getEngine()->scaleScene(x0, y0, z0, scaleX, scaleY, scaleZ);
}

void gcm::Mesh::applyRheology(RheologyCalculator* rc)
{
    for(int i = 0; i < getNodesNumber(); i++)
    {
        CalcNode& node = getNodeByLocalIndex(i);
        if( node.isLocal() )
            rc->doCalc(node, node);
    }
}

void gcm::Mesh::clearNodesState()
{
    for(int i = 0; i < getNodesNumber(); i++)
    {
        CalcNode& node = getNodeByLocalIndex(i);
        if( node.isLocal() )
            node.clearState();
    }
};

void gcm::Mesh::clearContactState()
{
    for(int i = 0; i < getNodesNumber(); i++)
    {
        CalcNode& node = getNodeByLocalIndex(i);
        if( node.isLocal() )
                        node.setInContact(false);
    }
}

void gcm::Mesh::processCrackState()
{
    for(int i = 0; i < getNodesNumber(); i++)
    {
        CalcNode& node = getNodeByLocalIndex(i);
        if( node.isLocal() )
        {
            float m_s[3];
            node.getMainStressComponents(m_s[0], m_s[1], m_s[2]);
            int i_ms=0; if (m_s[1]>m_s[i_ms]) i_ms=1; if (m_s[2]>m_s[i_ms]) i_ms = 2;
            if (m_s[i_ms] > node.getMaterial()->getCrackThreshold())
            {
                node.createCrack(i_ms);
                LOG_TRACE("New crack detected at node " << node);
            }
        }
    }
}

void gcm::Mesh::processCrackResponse()
{
    for(int i = 0; i < getNodesNumber(); i++)
    {
        CalcNode& node = getNodeByLocalIndex(i);
        if( node.isLocal() )
        {
            const vector3& m_s = node.getCrackDirection();
            // FIXME WA
            if (scalarProduct(m_s[0], m_s[1], m_s[2], m_s[0], m_s[1], m_s[2])>0.5)
            {
                node.cleanStressByDirection(m_s);
                LOG_TRACE("Existing crack found at node " << node);
            }
        }
    }
}

void gcm::Mesh::processStressState()
{
    // FIXME  remove these obsolete code since there is no necessary to recalculate
    // stresses components because now corresponding getter has lazy init stuff
    for(int i = 0; i < getNodesNumber(); i++)
    {
        CalcNode& node = getNodeByLocalIndex(i);
        if( node.isLocal() )
            node.calcMainStressComponents();
    }
}

void gcm::Mesh::moveCoords(float tau)
{
    LOG_DEBUG("Moving mesh coords");
    for(int i = 0; i < getNodesNumber(); i++)
    {
        CalcNode& node = getNodeByLocalIndex(i);
        if( node.isLocal() && node.isFirstOrder() )
        {
            CalcNode& newNode = getNewNode( node.number );
            for(int j = 0; j < 3; j++)
            {
                // Move node
                node.coords[j] += node.values[j]*tau;
                newNode.coords[j] = node.coords[j];
                // Move mesh outline if necessary
                // TODO - this does not 'clean' outline areas where there is no nodes anymore
                if(node.coords[j] > outline.max_coords[j])
                    outline.max_coords[j] = node.coords[j];
                if(node.coords[j] < outline.min_coords[j])
                    outline.min_coords[j] = node.coords[j];
                if(node.coords[j] > expandedOutline.max_coords[j])
                    expandedOutline.max_coords[j] = node.coords[j];
                if(node.coords[j] < expandedOutline.min_coords[j])
                    expandedOutline.min_coords[j] = node.coords[j];
            }
        }
    }
    calcMinH();
    LOG_DEBUG("New outline: " << outline);
};

float gcm::Mesh::getMaxEigenvalue()
{
    float maxLambda = 0;
    for(int i = 0; i < getNodesNumber(); i++)
    {
        CalcNode& node = getNodeByLocalIndex(i);
        RheologyMatrixPtr m = node.getRheologyMatrix();
        m->decomposeX(node);
        auto l1 = m->getMaxEigenvalue();
        m->decomposeY(node);
        auto l2 = m->getMaxEigenvalue();
        m->decomposeZ(node);
        auto l3 = m->getMaxEigenvalue();
        maxLambda = max({maxLambda, l1, l2, l3});
    }
    return maxLambda;
}

float gcm::Mesh::getMaxPossibleTimeStep()
{
    auto maxLambda = getMaxEigenvalue();
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
    for(int i = 0; i < getNodesNumber(); i++)
    {
        // FIXME this code seems to be dead
        // node = getNodeByLocalIndex(i);
        if( getNodeByLocalIndex(i).isLocal() )
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

bool gcm::Mesh::hasNode(int index)
{
    assert_ge(index, 0 );
    unordered_map<int, int>::const_iterator itr;
    itr = nodesMap.find(index);
    return itr != nodesMap.end();
}

CalcNode& gcm::Mesh::getNode(int index)
{
    assert_ge(index, 0 );
    unordered_map<int, int>::const_iterator itr;
    itr = nodesMap.find(index);
    assert_true(itr != nodesMap.end() );
    return nodes[itr->second];
}

CalcNode& gcm::Mesh::getNewNode(int index) {
    assert_ge(index, 0 );
    unordered_map<int, int>::const_iterator itr;
    itr = nodesMap.find(index);
    assert_true(itr != nodesMap.end() );
    return new_nodes[itr->second];
}

CalcNode& gcm::Mesh::getNodeByLocalIndex(unsigned int index) {
    assert_ge(index, 0);
    assert_lt(index, nodes.size());
    return nodes[index];
}

int gcm::Mesh::getNodeLocalIndex(int index) {
    assert_ge(index, 0 );
    unordered_map<int, int>::const_iterator itr;
    itr = nodesMap.find(index);
    return ( itr != nodesMap.end() ? itr->second : -1 );
}

void gcm::Mesh::addNode(CalcNode& node) {
    if( nodesNumber == nodesStorageSize )
        // FIXME what is this?
        // why not to use a propper allocator for container?
        createNodes((nodesStorageSize+1)*STORAGE_ONDEMAND_GROW_RATE);
    assert_lt(nodesNumber, nodesStorageSize );
    nodes[nodesNumber] = node;
    nodesMap[node.number] = nodesNumber;
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

    NumericalMethod *method = body->getEngine()->getNumericalMethod(numericalMethodType);
    method->setSpaceOrder(numericalMethodOrder);

    if( body->getEngine()->getNumberOfWorkers() != 1
            && ! syncedArea.includes( &areaOfInterest ) )
    {
        LOG_ERROR("Area of interest: " << areaOfInterest);
        LOG_ERROR("Synced area: " << syncedArea);
        assert_true(syncedArea.includes( &areaOfInterest ) );
    }

    // Border nodes
    LOG_DEBUG("Processing border nodes");
    for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
        int i = itr->first;
        CalcNode& node = getNode(i);
        if( node.isLocal() && node.isBorder() )
                method->doNextPartStep( node, getNewNode(i), tau, stage, this );
    }

    // Inner nodes
    LOG_DEBUG("Processing inner nodes");
    for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
        int i = itr->first;
        CalcNode& node = getNode(i);
        if( node.isLocal() && node.isInner() )
                method->doNextPartStep( node, getNewNode(i), tau, stage, this );
    }

    // Copy values
    LOG_DEBUG("Copying values");
    for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
        int i = itr->first;
        CalcNode& node = getNode(i);
        if( node.isLocal() )
            memcpy( node.values, getNewNode(i).values, VALUES_NUMBER * sizeof(float) );
    }
}
