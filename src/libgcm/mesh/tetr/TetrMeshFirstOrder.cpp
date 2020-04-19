#include "libgcm/mesh/tetr/TetrMeshFirstOrder.hpp"

#include <unordered_map>
#include <unordered_set>

#include "libgcm/node/CalcNode.hpp"

using namespace gcm;
using std::numeric_limits;
using std::unordered_map;
using std::make_pair;
using std::vector;

TetrMeshFirstOrder::TetrMeshFirstOrder()
{
    tetrsNumber = 0;
    tetrsStorageSize = 0;
    faceNumber = 0;
    mesh_min_h = numeric_limits<float>::infinity();
    mesh_max_h = numeric_limits<float>::infinity();
    mesh_avg_h = numeric_limits<float>::infinity();
    mesh_max_edge = numeric_limits<float>::infinity();
    // FIXME - hardcoded name
    numericalMethodType = "InterpolationFixedAxis";
    // FIXME - hardcoded parameter
    numericalMethodOrder = 1;
    INIT_LOGGER("gcm.TetrMeshFirstOrder");
    LOG_DEBUG("Creating mesh");
    cacheHits = 0;
    cacheMisses = 0;
    interpolator = new TetrFirstOrderInterpolator();
    kdtree = NULL;
}

TetrMeshFirstOrder::~TetrMeshFirstOrder()
{
    if( cacheHits > 0 || cacheMisses > 0 )
        LOG_DEBUG("CharactCache stats for mesh '" << getId() << "': "
                << cacheHits << " hits, " << cacheMisses << " misses. "
                << "Efficiency: " << ((float)cacheHits / (float)(cacheHits + cacheMisses)) );
    LOG_DEBUG("Destroying mesh '" << getId() << "'");
    // TODO - does it really trigger destructors?
    nodes.clear();
    new_nodes.clear();
    tetrs1.clear();
    border1.clear();
    delete interpolator;
    LOG_DEBUG("Mesh destroyed");
}

int TetrMeshFirstOrder::getTetrsNumber()
{
    return tetrsNumber;
}

int TetrMeshFirstOrder::getTriangleNumber()
{
    return faceNumber;
}

TetrFirstOrder& TetrMeshFirstOrder::getTetr(unsigned int index)
{
    if(USE_FAST_UNSAFE_SEARCH_FOR_TETRS) {
        return tetrs1[index];

    } else {
        assert_ge(index, 0);

        // Shortcut: index can almost always be used to get tetr from vector directly
        if (index < tetrs1.size()) {
            TetrFirstOrder &tetr = tetrs1[index];
            if (tetr.number == index)
                return tetr;
        }

        // If shortcut failed, use slow complete search
        unordered_map<int, int>::const_iterator itr;
        itr = tetrsMap.find(index);
        assert_true(itr != tetrsMap.end());
        return tetrs1[itr->second];
    }
}

bool TetrMeshFirstOrder::hasTetr(unsigned int index) {
    unordered_map<int, int>::const_iterator itr;
    itr = tetrsMap.find(index);
    return itr != tetrsMap.end();
}

TetrFirstOrder& TetrMeshFirstOrder::getTetrByLocalIndex(unsigned int index) {
    return tetrs1[index];
}

TriangleFirstOrder& TetrMeshFirstOrder::getTriangle(int index) {
    assert_ge(index, 0 );
    return border1[index];
}

void TetrMeshFirstOrder::createTetrs(int number) {
    LOG_DEBUG("Creating first order tetrs storage, size: " << (int)(number*STORAGE_OVERCOMMIT_RATIO));
    tetrs1.resize((int)(number*STORAGE_OVERCOMMIT_RATIO));
    tetrsStorageSize = number*STORAGE_OVERCOMMIT_RATIO;
}

void TetrMeshFirstOrder::createTriangles(int number) {
    LOG_DEBUG("Creating first order border storage, size: " << number);
    // TODO - make border working through addTriangle() / faceNumber++ / etc
    border1.resize(number);
    faceNumber = number;
    faceStorageSize = number;
}

void TetrMeshFirstOrder::addTetr(TetrFirstOrder& tetr) {
    if( tetrsNumber == tetrsStorageSize )
        createTetrs(tetrsStorageSize*STORAGE_ONDEMAND_GROW_RATE);
    assert_lt(tetrsNumber, tetrsStorageSize );
    tetrs1[tetrsNumber] = tetr;
    tetrsMap[tetr.number] = tetrsNumber;
    tetrsNumber++;
}

/*void TetrMeshFirstOrder::copyMesh(TetrMeshFirstOrder* src)
{
    LOG_DEBUG("Creating mesh using copy");
    int firstOrderNodesNumber = src->getNodesNumber();

    createNodes( firstOrderNodesNumber );
    for( int i = 0; i < firstOrderNodesNumber; i++ )
        addNode( src->getNode(i) );

    createTetrs(src->getTetrsNumber());

    for( int i = 0; i < src->getTetrsNumber(); i++ )
    {
        addTetr( src->getTetrByLocalIndex(i) );
    }
}*/

void TetrMeshFirstOrder::preProcessGeometry()
{
    LOG_DEBUG("Preprocessing mesh geometry started.");

    calcMaxH();
    calcAvgH();
    calcMaxEdge();

    verifyTetrahedraVertices ();
    build_volume_reverse_lookups();

    //check_unused_nodes();
    build_border();
    build_surface_reverse_lookups();

    check_numbering();
    // check_outer_normals();
    LOG_DEBUG("Preprocessing mesh geometry done.");
}

void TetrMeshFirstOrder::initSpatialIndex()
{
    if(kdtree != NULL)
        kd_free( kdtree );
    kdtree = kd_create(3);
    for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
        int i = itr->first;
        CalcNode& node = getNode(i);
        kd_insert3( kdtree, node.coords.x, node.coords.y, node.coords.z, &node );
    }
}

void TetrMeshFirstOrder::build_volume_reverse_lookups()
{
    LOG_DEBUG("Building volume reverse lookups");

    // Init vectors for "reverse lookups" of tetrahedrons current node is a member of.
    //for(int i = 0; i < nodesNumber; i++) {
    for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
        int i = itr->first;
        getVolumeElementsForNode(i).clear();
    }

    // Go through all the tetrahedrons
    //for(int i = 0; i < tetrsNumber; i++) {
    for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr ) {
        int i = itr->first;
        TetrFirstOrder& tetr = getTetr(i);
        // For all verticles
        for(int j = 0; j < 4; j++)
        {
            CalcNode& node = getNode( tetr.verts[j] );
            assert_true(node.isFirstOrder() );
            // Push to data of nodes the number of this tetrahedron
            getVolumeElementsForNode(i).push_back( tetr.number );
        }
    }
}

void TetrMeshFirstOrder::check_numbering()
{
    LOG_DEBUG("Checking numbering");

    // Check if internal numbers of nodes are the same as numbers in array
    // We need it in future to perform quick access to nodes in array
    //for(int i = 0; i < nodesNumber; i++)
    for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
        int i = itr->first;
        if(getNode(i).number != i)
            THROW_BAD_MESH("Invalid node numbering");
    }
    //for(int i = 0; i < tetrsNumber; i++) {
    for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr ) {
        int i = itr->first;
        if(getTetr(i).number != i)
            THROW_BAD_MESH("Invalid tetrahedron numbering");
    }
    for(int i = 0; i < faceNumber; i++)
    {
        if(getTriangle(i).number != i)
            THROW_BAD_MESH("Invalid triangle numbering");
    }
}

void TetrMeshFirstOrder::build_border()
{
    // Prepare border data

    float solid_angle;
    float solid_angle_part;

    LOG_DEBUG("Looking for border nodes using angles");
    int nodeCount = 0;

    // Check border using solid angle comparation with 4*PI

    //for( int i = 0; i < nodesNumber; i++ ) {
    for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
        int i = itr->first;
        CalcNode& node = getNode(i);
        node.setIsBorder(false);
        // We consider remote nodes to be 'border' of current local ones
        if( /*node.isLocal() &&*/ node.isFirstOrder() )
        {
            solid_angle = 0;
            vector<int>& elements = getVolumeElementsForNode(i);
            for(unsigned j = 0; j < elements.size(); j++)
            {
                solid_angle_part = get_solid_angle(i, elements[j]);
                assert_ge(solid_angle_part, 0);
                solid_angle += solid_angle_part;
            }
            if( fabs(4 * M_PI - solid_angle) > M_PI * EQUALITY_TOLERANCE ) {
                node.setIsBorder (true);
                nodeCount++;
            }
        }
    }
    LOG_DEBUG("Found " << nodeCount << " border nodes");

    LOG_DEBUG("Constructing border triangles");

    int faceCount = 0;
    int number = 0;

    // FIXME TODO - make faces uniq!

    // Check all tetrs and construct border triangles
    //for(int i = 0; i < tetrsNumber; i++) {
    for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr ) {
        int i = itr->first;
        TetrFirstOrder& tetr = getTetr(i);
        for( int j = 0; j < 4; j++ )
        {
            if( isTriangleBorder( tetr.verts ) )
                faceCount++;
            shiftArrayLeft( tetr.verts, 4 );
        }
    }

    LOG_DEBUG("Found " << faceCount << " border triangles");

    createTriangles(faceCount);

    //for(int i = 0; i < tetrsNumber; i++) {
    for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr ) {
        int i = itr->first;
        TetrFirstOrder& tetr = getTetr(i);
        for( int j = 0; j < 4; j++ )
        {
            if( isTriangleBorder( tetr.verts ) )
            {
                getTriangle(number) = createBorderTriangle( tetr.verts, number );
                number++;
            }
            shiftArrayLeft( tetr.verts, 4 );
        }
    }

    assert_eq(number, faceCount );
    LOG_DEBUG("Created " << faceNumber << " triangles");
}

void TetrMeshFirstOrder::build_surface_reverse_lookups()
{
    LOG_DEBUG("Building surface reverse lookups");

    // Init vectors for "reverse lookups" of border triangles current node is a member of.
    //for(int i = 0; i < nodesNumber; i++) {
    for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
        int i = itr->first;
        getBorderElementsForNode(i).clear();
    }

    // Go through all the triangles and push to data of nodes the number of this triangle
    for(int i = 0; i < faceNumber; i++)
    {
        TriangleFirstOrder& tri = getTriangle(i);
        for(int j = 0; j < 3; j++)
        {
            CalcNode& node = getNode( tri.verts[j] );
            assert_true(node.isFirstOrder() );
            getBorderElementsForNode(i).push_back( i );
        }
    }
}

void TetrMeshFirstOrder::check_unused_nodes()
{
    LOG_DEBUG("Looking for unused nodes");

    // Check all the nodes and find 'unused'

    // Case 1 - node has no connections at all
    //for(int i = 0; i < nodesNumber; i++) {
    for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
        int i = itr->first;
        CalcNode& node = getNode(i);
        if( getVolumeElementsForNode(i).size() == 0 )
            node.setUsed(false);
    }

    // Case 2 - remote ones that have connections only with remote ones
    //for(int i = 0; i < nodesNumber; i++) {
    /*for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
        int i = itr->first;
        CalcNode& node = getNode(i);
        // If node is remote
        if( node.isRemote() )
        {
            int count = 0;
            // Check tetrahedrons it is a member of
            for(unsigned j = 0; j < node.elements->size(); j++)
            {
                TetrFirstOrder& tetr = getTetr( node.elements->at(j) );
                // Check verticles
                for(int k = 0; k < 4; k++)
                {
                    // If it is local - count++
                    if( getNode( tetr.verts[k] ).isLocal() )
                        count++;
                }
            }
            // If remote node is NOT connected with at least one local - it is unused one
            if(count == 0)
            {
                node.setUsed(false);
            }
        }
    }*/
}

void TetrMeshFirstOrder::check_outer_normals()
{
    LOG_DEBUG("Checking nodes outer normals");

    // Normal vector
    float normal[3];
    // Displacement
    //float dx[3];

    // Guaranteed allowed step
    //float step_h = getMinH() * 0.5;

    // Check all nodes
    //for(int i = 0; i < nodesNumber; i++) {
    for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
        int i = itr->first;
        CalcNode& node = getNode(i);
        if(node.isBorder() && node.isUsed()) {

            findBorderNodeNormal(node, &normal[0], &normal[1], &normal[2], false);

            // Displacement along normal
            //dx[0] = step_h * normal[0];
            //dx[1] = step_h * normal[1];
            //dx[2] = step_h * normal[2];

            // Check if we are inside of the body moving towards normal
            //if( findOwnerTetr(&nodes[i], -dx[0], -dx[1], -dx[2]) == NULL ) {
                // Smth bad happens
//                *logger << "Can not find outer normal\n";
//                *logger << nodes[i].coords[0] << " " << nodes[i].coords[1] << " " < nodes[i].coords[2];
//                *logger << normal[0] << " " << normal[1] << " " < normal[2];
//                *logger << dx[0] << " " << dx[1] << " " < dx[2];
//                throw GCMException( GCMException::MESH_EXCEPTION, "Can not find outer normal");
            //} else {
                // Outside body - normal is outer - do nothing
            //}
        }

    }
    LOG_DEBUG("Checking nodes outer normals done");
}

void TetrMeshFirstOrder::verifyTetrahedraVertices ()
{
    LOG_DEBUG ("Verifying tetrahedra vertices");
    for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr ) {
        int tetrInd = itr->first;
        TetrFirstOrder& tetr = getTetr(tetrInd);
        for (int vertInd = 0; vertInd < 4; vertInd++)
        {
            assert_true(getNode( tetr.verts[vertInd] ).isFirstOrder() );
        }
    }
}

TriangleFirstOrder TetrMeshFirstOrder::createBorderTriangle(int v[4], int number)
{
    bool needSwap;
    bool isBorder;
    isBorder = isTriangleBorder( v, &needSwap );
    assert_true(isBorder );

    // Create and return trianlge
    TriangleFirstOrder tri;
    tri.number = number;
    if( !needSwap ) {
        tri.verts[0] = v[0];
        tri.verts[1] = v[1];
        tri.verts[2] = v[2];
    } else {
        tri.verts[0] = v[0];
        tri.verts[1] = v[2];
        tri.verts[2] = v[1];
    }
    return tri;
}

bool TetrMeshFirstOrder::isTriangleBorder(int v[4])
{
    bool needSwap;
    return isTriangleBorder( v, &needSwap, false );
}

bool TetrMeshFirstOrder::isTriangleBorder(int v[4], bool debug)
{
    bool needSwap;
    return isTriangleBorder( v, &needSwap, debug );
}

bool TetrMeshFirstOrder::isTriangleBorder(int v[4], bool* needSwap)
{
    return isTriangleBorder( v, needSwap, false );
}

bool TetrMeshFirstOrder::isTriangleBorder(int v[4], bool* needSwap, bool debug)
{
    *needSwap = false;
    CalcNode& v1 = getNode( v[0] );
    CalcNode& v2 = getNode( v[1] );
    CalcNode& v3 = getNode( v[2] );
    CalcNode& tetr_vert = getNode( v[3] );

    if( !v1.isBorder() || !v2.isBorder() || !v3.isBorder() )
        return false;

    bool _debug = debug;

    float h = getMinH() * 0.25;

    // Normal vector
    float normal[3];
    // Displacement
    float dx[3];

    float center[3];
    float innerDirection[3];
    float plane_move[3];
    // float plane_move_mod;

    findTriangleFaceNormal(v1.coords, v2.coords, v3.coords,
                            &normal[0], &normal[1], &normal[2]);

    // Triangle center
    for(int i = 0; i < 3; i++)
        center[i] = (v1.coords[i] + v2.coords[i] + v3.coords[i]) / 3;

    // Direction from triangle center to the verticle of tetrahedron
    for(int i = 0; i < 3; i++)
        innerDirection[i] = tetr_vert.coords[i] - center[i];

    if( _debug )
    {
        LOG_DEBUG("Debug issue with isTriangleBorder()");
        LOG_DEBUG("Node #1: " << v1);
        LOG_DEBUG("Node #2: " << v2);
        LOG_DEBUG("Node #3: " << v3);
        LOG_DEBUG("Add node: " << tetr_vert);
        LOG_DEBUG("Normal: " << normal[0] << " " << normal[1] << " " << normal[2]);
        LOG_DEBUG("Center: " << center[0] << " " << center[1] << " " << center[2]);
        LOG_DEBUG("Inner direction: " << innerDirection[0] << " " << innerDirection[1] << " " << innerDirection[2]);
        LOG_DEBUG("Scalar product: " << scalarProduct( normal, innerDirection));
        LOG_DEBUG("MinH: " << getMinH());
    }

    // Check if normal is co-linear with tetr verticle direction
    if( scalarProduct( normal, innerDirection) > 0 )
    {
        // If they are - invert normal because only opposite direction can be outer
        for(int i = 0; i < 3; i++)
            normal[i] = -normal[i];
        // And swap verticles order to match new direction
        //v2 = getNode( v[2] );
        //v3 = getNode( v[1] );
        *needSwap = true;
    }

    if( _debug )
    {
        LOG_DEBUG("Final normal: " << normal[0] << " " << normal[1] << " " << normal[2]);
    }

    // Displacement along potential outer normal
    for(int i = 0; i < 3; i++)
        dx[i] = h * normal[i];

    // Move from v1 to triangle center
    for(int i = 0; i < 3; i++)
        plane_move[i] = center[i] - v1.coords[i];

    for(int i = 0; i < 3; i++)
        dx[i] += plane_move[i];

    if( _debug )
    {
        LOG_DEBUG("Base node: " << v1);
        LOG_DEBUG("Move: " << dx[0] << " " << dx[1] << " " << dx[2]);
    }

    // Check if we are outside of the body moving from triangle center along normal ...
    if( fastScanForOwnerTetr(v1, dx[0], dx[1], dx[2], _debug) != -1 )
    {
        if(_debug)
            LOG_DEBUG("Result: not border");
        return false;
    }

    if(_debug)
        LOG_DEBUG("Result: border");

    return true;
};

int TetrMeshFirstOrder::findOwnerTetr(const CalcNode& node, float dx, float dy, float dz, bool debug, float* coords, bool* innerPoint)
{
    // Handle virt nodes
    if(node.getCustomFlag(CalcNode::FLAG_1))
    {
        assert_lt(node.number, 0);
        int testRes = virtExpandingScanForOwnerTetr(node, dx, dy, dz, debug, coords, innerPoint);
        return testRes;
    }
    assert_ge(node.number, 0);
    // Handle real nodes
    if( vectorSquareNorm(dx, dy, dz) < mesh_min_h * mesh_min_h * (1 - EQUALITY_TOLERANCE) )
    {
        int tetr = fastScanForOwnerTetr (node, dx, dy, dz, debug);
        coords[0] = node.coords[0] + dx;
        coords[1] = node.coords[1] + dy;
        coords[2] = node.coords[2] + dz;
        *innerPoint = (tetr != -1);
        return tetr;
    }
    else
    {
        //int newRes = newExpandingScanForOwnerTetr(node, dx, dy, dz, debug, coords, innerPoint);
        int orRes = orientedExpandingScanForOwnerTetr(node, dx, dy, dz, debug, coords, innerPoint);
        //int oldRes = expandingScanForOwnerTetr(node, dx, dy, dz, debug, coords, innerPoint);
        //int testRes = testExpandingScanForOwnerTetr(node, dx, dy, dz, debug, coords, innerPoint);
        //assert_eq(orRes, oldRes);
        return orRes;
    }
}

bool TetrMeshFirstOrder::belongsToTetr(int nodeNum, int tetrNum, int faceNum)
{
    int i1 = (0+faceNum) % 4;
    int i2 = (1+faceNum) % 4;
    int i3 = (2+faceNum) % 4;
    TetrFirstOrder& tetr = getTetr(tetrNum);
    if( (nodeNum == tetr.verts[i1]) || (nodeNum == tetr.verts[i2]) || (nodeNum == tetr.verts[i3]) )
        return true;
    else
        return false;
}

bool TetrMeshFirstOrder::isInnerPoint(const CalcNode& node, float dx, float dy, float dz, bool debug)
{
    bool innerPoint;
    float coords[3];
    findOwnerTetr(node, dx, dy, dz, debug, coords, &innerPoint);
    return innerPoint;
}

int TetrMeshFirstOrder::fastScanForOwnerTetr(const CalcNode& node, float dx, float dy, float dz, bool debug)
{
    if( debug )
        LOG_TRACE("Fast scan - debug ON (however, consider !inAABB condition)");
    //assert_le(vectorSquareNorm(dx, dy, dz), mesh_min_h * mesh_min_h );

    float x = node.coords[0] + dx;
    float y = node.coords[1] + dy;
    float z = node.coords[2] + dz;

    // Disable it since it causes issues with parallel computation
    // - it becomes not clear how we should use here outline, expandedOutline, 'zone of interest'
    if( ! outline.isInAABB(x, y, z) )
    {
        if( debug )
            LOG_TRACE("Not in AABB");
        return -1;
    }

    int res;
    if( charactCacheAvailable() )
    {
        if( checkCharactCache(node, dx, dy, dz, res) )
        {
            cacheHits++;
            return res;
        }
        cacheMisses++;
    }

    vector<int>& elements = getVolumeElementsForNode(node.number);

    for(unsigned i = 0; i < elements.size(); i++)
    {
        TetrFirstOrder& tetr = getTetr(elements[i]);
        if( pointInTetr(x, y, z,
                getNode( tetr.verts[0] ).coords, getNode( tetr.verts[1] ).coords,
                getNode( tetr.verts[2] ).coords, getNode( tetr.verts[3] ).coords,
                debug) )
        {
            updateCharactCache(node, dx, dy, dz, tetr.number);
            return tetr.number;
        }
    }

    updateCharactCache(node, dx, dy, dz, -1);
    return -1;
}

void TetrMeshFirstOrder::getPotentialOwnerTetrs(float x, float y, float z, vector<int>& tetrs)
{
    std::unordered_set<int> _nodes;
    std::unordered_set<int> _tetrs;
    double pt[3] = {x, y, z};
    double pos[3];
    CalcNode* pNode;
    struct kdres *presults = kd_nearest_range(kdtree, pt, getMaxEdge()*1.01);
    while (!kd_res_end(presults)) {
        /* get the data and position of the current result item */
        pNode = (CalcNode*) kd_res_item(presults, pos);
        _nodes.insert(pNode->number);
        /* go to the next entry */
        kd_res_next(presults);
    }
    kd_res_free( presults );
    for(int nodeNum : _nodes)
    {
        vector<int> volEls = getVolumeElementsForNode(nodeNum);
        for(int el : volEls) {
            TetrFirstOrder& t = getTetr(el);
            bool localN1 = (_nodes.find(t.verts[0]) != _nodes.end());
            bool localN2 = (_nodes.find(t.verts[1]) != _nodes.end());
            bool localN3 = (_nodes.find(t.verts[2]) != _nodes.end());
            bool localN4 = (_nodes.find(t.verts[3]) != _nodes.end());
            if(localN1 && localN2 && localN3 && localN4)
                _tetrs.insert(el);
        }
    }
    for(int t: _tetrs)
        tetrs.push_back(t);
}

int TetrMeshFirstOrder::newExpandingScanForOwnerTetr (const CalcNode& node, float dx, float dy, float dz, bool debug, float* coords, bool* innerPoint)
{
    float x = node.coords[0] + dx;
    float y = node.coords[1] + dy;
    float z = node.coords[2] + dz;

    if( debug ) {
        LOG_DEBUG("New expanding scan - debug ON");
        LOG_DEBUG("Base node: " << node);
        LOG_DEBUG("Move: " << dx << " " << dy << " " << dz);
        LOG_DEBUG("Target point: " << x << " " << y << " " << z);
    }

    if( !outline.isInAABB(x, y, z) ) {
        if( debug )
            LOG_DEBUG("Not in AABB");
        coords[0] = x;    coords[1] = y;    coords[2] = z;
        *innerPoint = false;
        updateCharactCache(node, dx, dy, dz, -1);
        return -1;
    }
    
    int res;
    if( charactCacheAvailable() )
    {
        if( checkCharactCache(node, dx, dy, dz, res) )
        {
            if( debug )
                LOG_DEBUG("Done using cache");
            cacheHits++;
            // Tell that point found is inner and set coords
            coords[0] = x;    coords[1] = y;    coords[2] = z;
            *innerPoint = true;
            return res;
        }
        cacheMisses++;
    }

    // A square of distance between point in question and local node
    // Will be used to check if it is worth to continue search or point in question is out of body
    float dist = vectorNorm(dx, dy, dz);
    float direction[3];
    direction[0] = dx / dist;
    direction[1] = dy / dist;
    direction[2] = dz / dist;
    
    if( debug ) {
        LOG_DEBUG("Dist: " << dist);
        LOG_DEBUG("Direction: " << direction[0] << " " << direction[1] << " " << direction[2]);
    }
    
    vector<int> tetrsToCheck;
    
    getPotentialOwnerTetrs(x, y, z, tetrsToCheck);

    // Check current tetrs
    for(unsigned i = 0; i < tetrsToCheck.size(); i++)
    {
        TetrFirstOrder& curTetr = getTetr(tetrsToCheck[i]);
        assert_eq(curTetr.number, tetrsToCheck[i] );
        CalcNode& n1 = getNode( curTetr.verts[0] );
        CalcNode& n2 = getNode( curTetr.verts[1] );
        CalcNode& n3 = getNode( curTetr.verts[2] );
        CalcNode& n4 = getNode( curTetr.verts[3] );
            
        if( debug ) {
            LOG_DEBUG("Checking tetr: " << curTetr);
            LOG_DEBUG("V1: " << n1);
            LOG_DEBUG("V2: " << n2);
            LOG_DEBUG("V3: " << n3);
            LOG_DEBUG("V4: " << n4);
        }

        // Check inside points of current tetr
        if( pointInTetr(x, y, z, n1.coords, n2.coords, n3.coords, n4.coords, debug) ) {
            // Tell that point found is inner and set coords
            coords[0] = x;    coords[1] = y;    coords[2] = z;
            *innerPoint = true;
            if( debug ) {
                LOG_DEBUG("Point is inner for tetr " << curTetr.number);
                LOG_DEBUG("Expanding scan done");
            }
            updateCharactCache(node, dx, dy, dz, curTetr.number);
            return curTetr.number;
        }
            
        if( debug )
            LOG_DEBUG("Point is not in tetr");

        // Check faces of current tetr
        for(int j = 0; j < 4; j++) {
            int i1 = (0+j) % 4;
            int i2 = (1+j) % 4;
            int i3 = (2+j) % 4;

            CalcNode& n1 = getNode( curTetr.verts[i1] );
            CalcNode& n2 = getNode( curTetr.verts[i2] );
            CalcNode& n3 = getNode( curTetr.verts[i3] );

            if( debug ) {
                LOG_DEBUG("Checking face");
                LOG_DEBUG("V1: " << n1);
                LOG_DEBUG("V2: " << n2);
                LOG_DEBUG("V3: " << n3);
            }

            if( pointInTriangle( node.coords[0], node.coords[1], node.coords[2],
                                    n1.coords, n2.coords, n3.coords, debug ) ) {
                if( debug )
                    LOG_DEBUG("Skipping");
                continue;
            }

            bool intersectionCheckRequired = ( n1.isBorder() && n2.isBorder() && n3.isBorder() );
                
            if( debug )
                LOG_DEBUG("Intersection check: " << intersectionCheckRequired);

            float tmpCoords[3];
            tmpCoords[0] = coords[0];
            tmpCoords[1] = coords[1];
            tmpCoords[2] = coords[2];
            float nodeCoords[3];
            nodeCoords[0] = node.coords.x;
            nodeCoords[1] = node.coords.y;
            nodeCoords[2] = node.coords.z;
            if( intersectionCheckRequired 
               && vectorIntersectsTriangle( n1.coords, n2.coords, n3.coords,
               nodeCoords, direction, dist, tmpCoords, debug))
            {
                if( debug )
                    LOG_DEBUG("Intersects");
                // Tell that point found is border,
                // coords are already set by vectorIntersectsTriangle()
                coords[0] = tmpCoords[0];
                coords[1] = tmpCoords[1];
                coords[2] = tmpCoords[2];
                *innerPoint = false;
                if( debug ) {
                    LOG_DEBUG("Intersection node: " << coords[0] << " " << coords[1] << " " << coords[2] );
                    LOG_DEBUG("Expanding scan done");
                }
                updateCharactCache(node, dx, dy, dz, -1);
                return curTetr.number;
            }
        }
    }

    coords[0] = x;    coords[1] = y;    coords[2] = z;
    *innerPoint = false;
    if( debug )
        LOG_DEBUG("Point not found");
    
    updateCharactCache(node, dx, dy, dz, -1);
    return -1;
}

// Older implementation based on 'real' expanding scan.
// The idea is nice, but implementation was still not good after few years of never-ending debug.
// (a) Clean impl was too slow. (b) Tricky impl missed nodes sometimes.
int TetrMeshFirstOrder::expandingScanForOwnerTetr (const CalcNode& node, float dx, float dy, float dz, bool debug, float* coords, bool* innerPoint)
{
    float x = node.coords[0] + dx;
    float y = node.coords[1] + dy;
    float z = node.coords[2] + dz;

    if( debug ) {
        LOG_DEBUG("Expanding scan - debug ON");
        LOG_DEBUG("Base node: " << node);
        LOG_DEBUG("Move: " << dx << " " << dy << " " << dz);
        LOG_DEBUG("Target point: " << x << " " << y << " " << z);
    }
    
    if( !outline.isInAABB(x, y, z) ) {
        if( debug )
            LOG_DEBUG("Not in AABB");
        coords[0] = x;    coords[1] = y;    coords[2] = z;
        *innerPoint = false;
        updateCharactCache(node, dx, dy, dz, -1);
        return -1;
    }
    
    int res;
    if( charactCacheAvailable() )
    {
        if( checkCharactCache(node, dx, dy, dz, res) )
        {
            if( debug )
                LOG_DEBUG("Done using cache");
            cacheHits++;
            // Tell that point found is inner and set coords
            coords[0] = x;    coords[1] = y;    coords[2] = z;
            *innerPoint = true;
            return res;
        }
        cacheMisses++;
    }

    // A square of distance between point in question and local node
    // Will be used to check if it is worth to continue search or point in question is out of body
    float R2 = vectorSquareNorm(dx, dy, dz);

    //if( R2 <= mesh_min_h * mesh_min_h )
    //{
        //LOG_WARN("Base node: " << *node);
        //LOG_WARN("Direction: " << dx << " " << dy << " " << dz);
        //LOG_WARN("Expanding scan started for small distance: most probably, smth is wrong with normals");
        //THROW_BAD_MESH("Expanding scan started for small distance: most probably, smth is wrong with normals");
    //}

    float dist = sqrt(R2);
    float direction[3];
    direction[0] = dx / dist;
    direction[1] = dy / dist;
    direction[2] = dz / dist;
    
    if( debug ) {
        LOG_DEBUG("Dist: " << dist);
        LOG_DEBUG("Direction: " << direction[0] << " " << direction[1] << " " << direction[2]);
        LOG_DEBUG("R2: " << R2);
    }

    vector<int> tetrsToCheck;
    unordered_map<int, bool> checkedWithNeigh;
    unordered_map<int, bool>::const_iterator itr;

    // If current tetrs are inside sphere of radius R or not. If not - we should stop search and return NULL
    bool inside_R = true;
    bool last_inside_R = true;

    bool triangleIntersectionFound;

    float local_x, local_y, local_z;

    vector<int>& elements = getVolumeElementsForNode(node.number);

    for(unsigned i = 0; i < elements.size(); i++)
        tetrsToCheck.push_back( elements[i] );

    while(inside_R)
    {
        if( debug )
            LOG_DEBUG("Starting next stage of scan");

        inside_R = last_inside_R;
        last_inside_R = false;
        triangleIntersectionFound = false;

        // Check current tetrs
        for(unsigned i = 0; i < tetrsToCheck.size(); i++)
        {
            TetrFirstOrder& curTetr = getTetr(tetrsToCheck[i]);
            assert_eq(curTetr.number, tetrsToCheck[i] );
            CalcNode& n1 = getNode( curTetr.verts[0] );
            CalcNode& n2 = getNode( curTetr.verts[1] );
            CalcNode& n3 = getNode( curTetr.verts[2] );
            CalcNode& n4 = getNode( curTetr.verts[3] );
            
            if( debug ) {
                LOG_DEBUG("Checking tetr: " << curTetr);
                LOG_DEBUG("V1: " << n1);
                LOG_DEBUG("V2: " << n2);
                LOG_DEBUG("V3: " << n3);
                LOG_DEBUG("V4: " << n4);
            }

            // Check inside points of current tetr
            if( pointInTetr(x, y, z, n1.coords, n2.coords, n3.coords, n4.coords, debug) ) {
                // Tell that point found is inner and set coords
                coords[0] = x;    coords[1] = y;    coords[2] = z;
                *innerPoint = true;
                if( debug )
                {
                    LOG_DEBUG("Point is inner for tetr " << curTetr.number);
                    LOG_DEBUG("Expanding scan done");
                }
                //return curTetr;
                updateCharactCache(node, dx, dy, dz, curTetr.number);
                return curTetr.number;
            }
            
            if( debug )
                LOG_DEBUG("Point is not in tetr");

            // Check faces of current tetr
            for(int j = 0; j < 4; j++)
            {
                int i1 = (0+j) % 4;
                int i2 = (1+j) % 4;
                int i3 = (2+j) % 4;

                CalcNode& n1 = getNode( curTetr.verts[i1] );
                CalcNode& n2 = getNode( curTetr.verts[i2] );
                CalcNode& n3 = getNode( curTetr.verts[i3] );
                
                if( debug ) {
                    LOG_DEBUG("Checking face");
                    LOG_DEBUG("V1: " << n1);
                    LOG_DEBUG("V2: " << n2);
                    LOG_DEBUG("V3: " << n3);
                }

                if( pointInTriangle( node.coords[0], node.coords[1], node.coords[2],
                                        n1.coords, n2.coords, n3.coords, debug ) )
                {
                    triangleIntersectionFound = true; 
                    if( debug )
                        LOG_DEBUG("Skipping");
                    continue;
                }

                bool intersectionCheckRequired = (
                    ( n1.isBorder() && n2.isBorder() && n3.isBorder() )
                    || !triangleIntersectionFound
                );
                
                if( debug )
                    LOG_DEBUG("Intersection check: " << intersectionCheckRequired);

                float tmpCoords[3];
                tmpCoords[0] = coords[0];
                tmpCoords[1] = coords[1];
                tmpCoords[2] = coords[2];
                float nodeCoords[3];
                nodeCoords[0] = node.coords.x;
                nodeCoords[1] = node.coords.y;
                nodeCoords[2] = node.coords.z;
                if( intersectionCheckRequired
                        && vectorIntersectsTriangle( n1.coords, n2.coords, n3.coords,
                                                    nodeCoords, direction, dist, tmpCoords, debug)
                )
                {
                    if( debug )
                        LOG_DEBUG("Intersects");
                    if ( n1.isBorder() && n2.isBorder() && n3.isBorder() )
                    {
                        // Tell that point found is border,
                        // coords are already set by vectorIntersectsTriangle()
                        coords[0] = tmpCoords[0];
                        coords[1] = tmpCoords[1];
                        coords[2] = tmpCoords[2];
                        *innerPoint = false;
                        if( debug )
                        {
                            LOG_DEBUG("Intersection node: " << coords[0] << " " << coords[1] << " " << coords[2] );
                            LOG_DEBUG("Expanding scan done");
                        }
                        //return curTetr;
                        updateCharactCache(node, dx, dy, dz, -1);
                        return curTetr.number;
                    }
                    else
                    {
                        triangleIntersectionFound = true;
                    }
                }
            }

            checkedWithNeigh[ tetrsToCheck[i] ] = false;

            // Check if this tetr is still inside sphere of radius R
            // If we have already found at least one tetr in the sphere - skip check
            if(!last_inside_R)
            {
                if( debug )
                    LOG_DEBUG("Checking if we are inside R2 sphere");
                // For all verticles of current tetr
                for(int j = 0; j < 4; j++)
                {
                    CalcNode& vertJ = getNode( curTetr.verts[j] );
                    // Skip base node. Otherwise we'll get false positive insideR for the 1st and 2nd layers
                    if( vertJ.number == node.number )
                        continue;

                    local_x = vertJ.coords[0];
                    local_y = vertJ.coords[1];
                    local_z = vertJ.coords[2];
                    // If its distance smaller than R
                    if( vectorSquareNorm( node.coords[0] - local_x,
                            node.coords[1] - local_y, node.coords[2] - local_z) < R2 )
                    {
                            last_inside_R = true;
                    }
                    // FIXME In theory we whould check if sphere and tetr intersect.
                    // Current check - if at least one vert is in sphere.
                    // It can be wrong on turbo bad tetrs. It fails if
                    // sphere goes 'through' tetr to next layer but does not include its verticles.
                }
                if( debug )
                    LOG_DEBUG("Check result: " << last_inside_R);
            }
        }

        // If current layer is not in sphere - there is no need to create next layer - just return NULL
        if( !inside_R || ( node.isBorder() && !triangleIntersectionFound ) )
        {
            coords[0] = x;    coords[1] = y;    coords[2] = z;
            *innerPoint = false;
            if( debug )
                LOG_DEBUG("Point not found");
            updateCharactCache(node, dx, dy, dz, -1);
            return -1;
        }

        // If not found in current tetrs - create new list for checking
        int checkedTetrNum;
        bool isCompletelyChecked;
        int nextTetrNum;

        tetrsToCheck.clear();

        for( itr = checkedWithNeigh.begin(); itr != checkedWithNeigh.end(); ++itr )
        {
            checkedTetrNum = itr->first;
            isCompletelyChecked = itr->second;
            if( isCompletelyChecked )
                continue;

            TetrFirstOrder& checkedTetr = getTetr(checkedTetrNum);
            for(int j = 0; j < 4; j++)
            {
                CalcNode& vertJ = getNode( checkedTetr.verts[j] );
                vector<int>& elements = getVolumeElementsForNode(vertJ.number);
                for(unsigned k = 0; k < elements.size(); k++)
                {
                    nextTetrNum = elements[k];
                    if( checkedWithNeigh.find(nextTetrNum) == checkedWithNeigh.end() )
                        tetrsToCheck.push_back(nextTetrNum);
                }
            }
            checkedWithNeigh[checkedTetrNum] = true;
        }
    }

    coords[0] = x;    coords[1] = y;    coords[2] = z;
    *innerPoint = false;
    if( debug )
        LOG_DEBUG("Point not found");
    
    updateCharactCache(node, dx, dy, dz, -1);
    return -1;
}

int TetrMeshFirstOrder::orientedExpandingScanForOwnerTetr (const CalcNode& node, float dx, float dy, float dz, bool debug, float* coords, bool* innerPoint)
{
    float x = node.coords[0] + dx;
    float y = node.coords[1] + dy;
    float z = node.coords[2] + dz;

    if( debug ) {
        LOG_DEBUG("Oriented expanding scan - debug ON");
        LOG_DEBUG("Base node: " << node);
        LOG_DEBUG("Move: " << dx << " " << dy << " " << dz);
        LOG_DEBUG("Target point: " << x << " " << y << " " << z);
    }
    
    if( !outline.isInAABB(x, y, z) ) {
        if( debug )
            LOG_DEBUG("Not in AABB");
        coords[0] = x;    coords[1] = y;    coords[2] = z;
        *innerPoint = false;
        updateCharactCache(node, dx, dy, dz, -1);
        return -1;
    }
    
    int res;
    if( charactCacheAvailable() )
    {
        if( checkCharactCache(node, dx, dy, dz, res) )
        {
            if( debug )
                LOG_DEBUG("Done using cache");
            cacheHits++;
            // Tell that point found is inner and set coords
            coords[0] = x;    coords[1] = y;    coords[2] = z;
            *innerPoint = true;
            return res;
        }
        cacheMisses++;
    }

    // A square of distance between point in question and local node
    // Will be used to check if it is worth to continue search or point in question is out of body
    float R2 = vectorSquareNorm(dx, dy, dz);
    float dist = sqrt(R2);
    float direction[3];
    direction[0] = dx / dist;
    direction[1] = dy / dist;
    direction[2] = dz / dist;
    
    if( debug ) {
        LOG_DEBUG("Dist: " << dist);
        LOG_DEBUG("Direction: " << direction[0] << " " << direction[1] << " " << direction[2]);
        LOG_DEBUG("R2: " << R2);
    }

    bool shellIntersectionFound = false;
    vector<vector<int>> facesCrossed;
    unordered_map<int, bool> tetrsChecked;

    vector<int>& elements = getVolumeElementsForNode(node.number);

    // Check tetrs around
    for(unsigned i = 0; i < elements.size(); i++)
    {
        TetrFirstOrder& curTetr = getTetr(elements[i]);
        CalcNode& n1 = getNode( curTetr.verts[0] );
        CalcNode& n2 = getNode( curTetr.verts[1] );
        CalcNode& n3 = getNode( curTetr.verts[2] );
        CalcNode& n4 = getNode( curTetr.verts[3] );
            
        if( debug ) {
            LOG_DEBUG("Checking tetr: " << curTetr);
            LOG_DEBUG("V1: " << n1);
            LOG_DEBUG("V2: " << n2);
            LOG_DEBUG("V3: " << n3);
            LOG_DEBUG("V4: " << n4);
        }

        // Check inside points of current tetr
        if( pointInTetr(x, y, z, n1.coords, n2.coords, n3.coords, n4.coords, debug) ) {
            // Tell that point found is inner and set coords
            coords[0] = x;    coords[1] = y;    coords[2] = z;
            *innerPoint = true;
            if( debug )
            {
                LOG_DEBUG("Point is inner for tetr " << curTetr.number);
                LOG_DEBUG("Expanding scan done");
            }
            updateCharactCache(node, dx, dy, dz, curTetr.number);
            return curTetr.number;
        }
            
        if( debug )
            LOG_DEBUG("Point is not in tetr");

        // Check faces of current tetr
        for(int j = 0; j < 4; j++)
        {
            int i1 = (0+j) % 4;
            int i2 = (1+j) % 4;
            int i3 = (2+j) % 4;

            CalcNode& n1 = getNode( curTetr.verts[i1] );
            CalcNode& n2 = getNode( curTetr.verts[i2] );
            CalcNode& n3 = getNode( curTetr.verts[i3] );
                
            if( debug ) {
                LOG_DEBUG("Checking face");
                LOG_DEBUG("V1: " << n1);
                LOG_DEBUG("V2: " << n2);
                LOG_DEBUG("V3: " << n3);
            }

            // We should check only faces that form the 'semi-sphere' around base node
            //if( pointInTriangle( node.coords[0], node.coords[1], node.coords[2],
            //                        n1.coords, n2.coords, n3.coords, debug ) )
            if( belongsToTetr(node.number, curTetr.number, j) )
            {
                if( debug )
                    LOG_DEBUG("Skipping");
                continue;
            }

            float nodeCoords[3];
            nodeCoords[0] = node.coords.x;
            nodeCoords[1] = node.coords.y;
            nodeCoords[2] = node.coords.z;
            if( vectorIntersectsTriangle( n1.coords, n2.coords, n3.coords,
                                                nodeCoords, direction, dist, coords, debug) )
            {
                if( debug ) {
                    LOG_DEBUG("Intersects");
                    LOG_DEBUG("V1: " << n1);
                    LOG_DEBUG("V2: " << n2);
                    LOG_DEBUG("V3: " << n3);
                }
                if ( n1.isBorder() && n2.isBorder() && n3.isBorder() )
                {
                    // Tell that point found is border,
                    // coords are already set by vectorIntersectsTriangle()
                    *innerPoint = false;
                    if( debug )
                    {
                        LOG_DEBUG("Intersection node: " << coords[0] << " " << coords[1] << " " << coords[2] );
                        LOG_DEBUG("Expanding scan done");
                    }
                    updateCharactCache(node, dx, dy, dz, -1);
                    return curTetr.number;
                }
                else
                {
                    shellIntersectionFound = true;
                    vector<int> faceVerts;
                    faceVerts.push_back(n1.number);
                    faceVerts.push_back(n2.number);
                    faceVerts.push_back(n3.number);
                    facesCrossed.push_back(faceVerts);
                    tetrsChecked[curTetr.number] = true;
                    if( debug ) {
                        LOG_DEBUG("Shell intersection found.");
                    }
                    break;
                }
            }
        }
        if( shellIntersectionFound )
            break;
    }
    
    if(!shellIntersectionFound) {
        coords[0] = x;    coords[1] = y;    coords[2] = z;
        *innerPoint = false;
        if( debug )
            LOG_DEBUG("Point not found");
    
        updateCharactCache(node, dx, dy, dz, -1);
        return -1;
    }
    
    if(facesCrossed.size() == 0) {
        if(!debug) {
            LOG_DEBUG("EXTENDED DEBUG INFO START");
            orientedExpandingScanForOwnerTetr(node, dx, dy, dz, true, coords, innerPoint);
            LOG_DEBUG("EXTENDED DEBUG INFO END");
        }
        THROW_BAD_MESH("facesCrossed == 0");
    }
    
    // Oriented scan
    while( facesCrossed.size() < 10 )
    {
        int nextTetr = -1;

        vector<int> lastFace = facesCrossed[facesCrossed.size()-1];
        int v1 = lastFace[0];
        int v2 = lastFace[1];
        int v3 = lastFace[2];
        if( debug )
            LOG_DEBUG("Looking for the next tetr to check. Face: " << v1 << " " << v2 << " " << v3);
        for(int j = 0; j < 3; j++) {
            vector<int>& nextElems = getVolumeElementsForNode(lastFace[j]);
            for(unsigned i = 0; i < nextElems.size(); i++) {
                TetrFirstOrder& possibleTetr = getTetr(nextElems[i]);
                if( debug ) {
                    LOG_DEBUG("Checking tetr: " << possibleTetr);
                    /*CalcNode& n1 = getNode( possibleTetr.verts[0] );
                    CalcNode& n2 = getNode( possibleTetr.verts[1] );
                    CalcNode& n3 = getNode( possibleTetr.verts[2] );
                    CalcNode& n4 = getNode( possibleTetr.verts[3] );
            
                    LOG_DEBUG("V1: " << n1);
                    LOG_DEBUG("V2: " << n2);
                    LOG_DEBUG("V3: " << n3);
                    LOG_DEBUG("V4: " << n4);*/
                }
                // Skip tetrs we have cheched already
                if(tetrsChecked.find(possibleTetr.number) != tetrsChecked.end()) {
                    if( debug )
                        LOG_DEBUG("Already checked");
                    continue;
                }
                if( debug )
                    LOG_DEBUG("New tetr. Checking verts.");
                bool v1Exists = ( (v1 == possibleTetr.verts[0]) || (v1 == possibleTetr.verts[1]) 
                                    || (v1 == possibleTetr.verts[2]) || (v1 == possibleTetr.verts[3]));
                bool v2Exists = ( (v2 == possibleTetr.verts[0]) || (v2 == possibleTetr.verts[1]) 
                                    || (v2 == possibleTetr.verts[2]) || (v2 == possibleTetr.verts[3]));
                bool v3Exists = ( (v3 == possibleTetr.verts[0]) || (v3 == possibleTetr.verts[1]) 
                                    || (v3 == possibleTetr.verts[2]) || (v3 == possibleTetr.verts[3]));
                if( debug )
                    LOG_DEBUG("Verts res:" << v1Exists << " " << v2Exists << " " << v3Exists);
                if(v1Exists && v2Exists && v3Exists) {
                    if( debug )
                        LOG_DEBUG("Found. Next tetr: " << possibleTetr.number);
                    nextTetr = possibleTetr.number;
                    break;
                }
            }
            if(nextTetr != -1)
                break;
        }
        if( debug )
            LOG_DEBUG("Next tetr: " << nextTetr);

        if(nextTetr < 0) {
            /*if(!debug) {
                LOG_DEBUG("EXTENDED DEBUG INFO START");
                orientedExpandingScanForOwnerTetr(node, dx, dy, dz, true, coords, innerPoint);
                LOG_DEBUG("EXTENDED DEBUG INFO END");
            }
            THROW_BAD_MESH("nextTetr not found for oriented search");*/
            if( debug )
                LOG_DEBUG("nextTetr not found for oriented search");
            coords[0] = x;    coords[1] = y;    coords[2] = z;
            *innerPoint = false;
            updateCharactCache(node, dx, dy, dz, -1);
            return -1;
        }


        TetrFirstOrder& curTetr = getTetr(nextTetr);
        CalcNode& n1 = getNode( curTetr.verts[0] );
        CalcNode& n2 = getNode( curTetr.verts[1] );
        CalcNode& n3 = getNode( curTetr.verts[2] );
        CalcNode& n4 = getNode( curTetr.verts[3] );

        if( debug ) {
            LOG_DEBUG("Checking tetr: " << curTetr);
            LOG_DEBUG("V1: " << n1);
            LOG_DEBUG("V2: " << n2);
            LOG_DEBUG("V3: " << n3);
            LOG_DEBUG("V4: " << n4);
        }

        // Check inside points of current tetr
        if( pointInTetr(x, y, z, n1.coords, n2.coords, n3.coords, n4.coords, debug) ) {
            // Tell that point found is inner and set coords
            coords[0] = x;    coords[1] = y;    coords[2] = z;
            *innerPoint = true;
            if( debug )
            {
                LOG_DEBUG("Point is inner for tetr " << curTetr.number);
                LOG_DEBUG("Expanding scan done");
            }
            updateCharactCache(node, dx, dy, dz, curTetr.number);
            return curTetr.number;
        }

        if( debug )
            LOG_DEBUG("Point is not in tetr");
        
        bool intersectionFound = false;

        // Check faces of current tetr
        for(int j = 0; j < 4; j++)
        {
            int i1 = (0+j) % 4;
            int i2 = (1+j) % 4;
            int i3 = (2+j) % 4;

            CalcNode& n1 = getNode( curTetr.verts[i1] );
            CalcNode& n2 = getNode( curTetr.verts[i2] );
            CalcNode& n3 = getNode( curTetr.verts[i3] );

            if( debug ) {
                LOG_DEBUG("Checking face");
                LOG_DEBUG("V1: " << n1);
                LOG_DEBUG("V2: " << n2);
                LOG_DEBUG("V3: " << n3);
            }

            // We should check only faces that form the 'semi-sphere' around base node
            // TODO:
            /*if( pointInTriangle( node.coords[0], node.coords[1], node.coords[2],
                                    n1.coords, n2.coords, n3.coords, debug ) )
            {
                if( debug )
                    LOG_DEBUG("Skipping");
                continue;
            }*/

            // Skip faces we checked before
            bool faceChecked = false;
            for(int k = 0; k < facesCrossed.size(); k++)
            {
                vector<int> face = facesCrossed[k];
                int v1 = face[0];
                int v2 = face[1];
                int v3 = face[2];
                if( debug ) {
                    LOG_DEBUG("Crossed face: " << v1 << " " << v2 << " " << v3);
                }
                bool v1Exists = ( (v1 == n1.number) || (v1 == n2.number) || (v1 == n3.number) );
                bool v2Exists = ( (v2 == n1.number) || (v2 == n2.number) || (v2 == n3.number) );
                bool v3Exists = ( (v3 == n1.number) || (v3 == n2.number) || (v3 == n3.number) );
                faceChecked = v1Exists && v2Exists && v3Exists;
                if( debug ) {
                    LOG_DEBUG("Checks: " << faceChecked << " / " << v1Exists << " " << v2Exists << " " << v3Exists);
                }
            }
            if(faceChecked)
                continue;

            float nodeCoords[3];
            nodeCoords[0] = node.coords.x;
            nodeCoords[1] = node.coords.y;
            nodeCoords[2] = node.coords.z;
            if( vectorIntersectsTriangle( n1.coords, n2.coords, n3.coords,
                                                nodeCoords, direction, dist, coords, debug) )
            {
                intersectionFound = true;
                if( debug )
                    LOG_DEBUG("Intersects");
                if ( n1.isBorder() && n2.isBorder() && n3.isBorder() )
                {
                    // Tell that point found is border,
                    // coords are already set by vectorIntersectsTriangle()
                    *innerPoint = false;
                    if( debug )
                    {
                        LOG_DEBUG("Intersection node: " << coords[0] << " " << coords[1] << " " << coords[2] );
                        LOG_DEBUG("Expanding scan done");
                    }
                    updateCharactCache(node, dx, dy, dz, -1);
                    return curTetr.number;
                }
                else
                {
                    if( debug ) {
                        LOG_DEBUG("Shell intersection found.");
                    }
                    vector<int> faceVerts;
                    faceVerts.push_back(n1.number);
                    faceVerts.push_back(n2.number);
                    faceVerts.push_back(n3.number);
                    facesCrossed.push_back(faceVerts);
                    // TODO: here? or above? and the same on the first run
                    tetrsChecked[curTetr.number] = true;
                    break;
                }
            }
        }
        if(!intersectionFound) {
            /*if(!debug) {
                LOG_DEBUG("EXTENDED DEBUG INFO START");
                orientedExpandingScanForOwnerTetr(node, dx, dy, dz, true, coords, innerPoint);
                LOG_DEBUG("EXTENDED DEBUG INFO END");
            }*/
            //THROW_BAD_MESH("Face intersection not found during oriented search");
            if( debug )
                LOG_DEBUG("Face intersection not found during oriented search");
            coords[0] = x;    coords[1] = y;    coords[2] = z;
            *innerPoint = false;
            updateCharactCache(node, dx, dy, dz, -1);
            return -1;
        }
    }
    
    THROW_BAD_MESH("Oriented scan takes too long - smth went wrong");
}

int TetrMeshFirstOrder::virtExpandingScanForOwnerTetr (const CalcNode& _node, float _dx, float _dy, float _dz, bool debug, float* coords, bool* innerPoint)
{
    float x = _node.coords[0] + _dx;
    float y = _node.coords[1] + _dy;
    float z = _node.coords[2] + _dz;

    if( debug ) {
        LOG_DEBUG("Expanding scan - debug ON");
        LOG_DEBUG("Node: " << _node);
        LOG_DEBUG("Move: " << _dx << " " << _dy << " " << _dz);
        LOG_DEBUG("Target point: " << x << " " << y << " " << z);
    }

    double pt[3] = {x, y, z};
    double pos[3];
    CalcNode* pNode;
    struct kdres *presults = kd_nearest(kdtree, pt);
    pNode = (CalcNode*) kd_res_item(presults, pos);
    kd_res_free( presults );
    
    CalcNode& node = getNode(pNode->number);
    float dx = _node.coords.x - node.coords.x + _dx;
    float dy = _node.coords.y - node.coords.y + _dy;
    float dz = _node.coords.z - node.coords.z + _dz;

    if( !outline.isInAABB(x, y, z) ) {
        if( debug )
            LOG_DEBUG("Not in AABB");
        coords[0] = x;    coords[1] = y;    coords[2] = z;
        *innerPoint = false;
        updateCharactCache(node, dx, dy, dz, -1);
        return -1;
    }
    
    if( debug ) {
        LOG_DEBUG("Determined base node: " << node);
        LOG_DEBUG("New move: " << dx << " " << dy << " " << dz);
    }
    
    return orientedExpandingScanForOwnerTetr(node, dx, dy, dz, debug, coords, innerPoint);
    
    int res;
    if( charactCacheAvailable() )
    {
        if( checkCharactCache(node, dx, dy, dz, res) )
        {
            if( debug )
                LOG_DEBUG("Done using cache");
            cacheHits++;
            // Tell that point found is inner and set coords
            coords[0] = x;    coords[1] = y;    coords[2] = z;
            *innerPoint = true;
            return res;
        }
        cacheMisses++;
    }

    // A square of distance between point in question and local node
    // Will be used to check if it is worth to continue search or point in question is out of body
    float R2 = vectorSquareNorm(dx, dy, dz);

    if( R2 <= mesh_min_h * mesh_min_h )
    {
        //LOG_WARN("Base node: " << *node);
        //LOG_WARN("Direction: " << dx << " " << dy << " " << dz);
        //LOG_WARN("Expanding scan started for small distance: most probably, smth is wrong with normals");
        //THROW_BAD_MESH("Expanding scan started for small distance: most probably, smth is wrong with normals");
    }

    float dist = sqrt(R2);
    float direction[3];
    direction[0] = dx / dist;
    direction[1] = dy / dist;
    direction[2] = dz / dist;
    
    if( debug ) {
        LOG_DEBUG("Dist: " << dist);
        LOG_DEBUG("Direction: " << direction[0] << " " << direction[1] << " " << direction[2]);
        LOG_DEBUG("R2: " << R2);
    }

    // if( ! outline.isInAABB(x, y, z) )
    //    return NULL;

    vector<int> tetrsToCheck;
    unordered_map<int, bool> checkedWithNeigh;
    unordered_map<int, bool>::const_iterator itr;

    // If current tetrs are inside sphere of radius R or not. If not - we should stop search and return NULL
    bool inside_R = true;
    bool last_inside_R = true;

    bool triangleIntersectionFound;

    float local_x, local_y, local_z;

    vector<int>& elements = getVolumeElementsForNode(node.number);

    for(unsigned i = 0; i < elements.size(); i++)
        tetrsToCheck.push_back( elements[i] );

    while(inside_R)
    {
        if( debug )
            LOG_DEBUG("Starting next stage of scan");

        inside_R = last_inside_R;
        last_inside_R = false;
        triangleIntersectionFound = false;

        // Check current tetrs
        for(unsigned i = 0; i < tetrsToCheck.size(); i++)
        {
            TetrFirstOrder& curTetr = getTetr(tetrsToCheck[i]);
            assert_eq(curTetr.number, tetrsToCheck[i] );
            CalcNode& n1 = getNode( curTetr.verts[0] );
            CalcNode& n2 = getNode( curTetr.verts[1] );
            CalcNode& n3 = getNode( curTetr.verts[2] );
            CalcNode& n4 = getNode( curTetr.verts[3] );
            
            if( debug ) {
                LOG_DEBUG("Checking tetr: " << curTetr);
                LOG_DEBUG("V1: " << n1);
                LOG_DEBUG("V2: " << n2);
                LOG_DEBUG("V3: " << n3);
                LOG_DEBUG("V4: " << n4);
            }

            // Check inside points of current tetr
            if( pointInTetr(x, y, z, n1.coords, n2.coords, n3.coords, n4.coords, debug) ) {
                // Tell that point found is inner and set coords
                coords[0] = x;    coords[1] = y;    coords[2] = z;
                *innerPoint = true;
                if( debug )
                {
                    LOG_DEBUG("Point is inner for tetr " << curTetr.number);
                    LOG_DEBUG("Expanding scan done");
                }
                //return curTetr;
                updateCharactCache(node, dx, dy, dz, curTetr.number);
                return curTetr.number;
            }
            
            if( debug )
                LOG_DEBUG("Point is not in tetr");

            // Check faces of current tetr
            for(int j = 0; j < 4; j++)
            {
                int i1 = (0+j) % 4;
                int i2 = (1+j) % 4;
                int i3 = (2+j) % 4;

                CalcNode& n1 = getNode( curTetr.verts[i1] );
                CalcNode& n2 = getNode( curTetr.verts[i2] );
                CalcNode& n3 = getNode( curTetr.verts[i3] );
                
                if( debug ) {
                    LOG_DEBUG("Checking face");
                    LOG_DEBUG("V1: " << n1);
                    LOG_DEBUG("V2: " << n2);
                    LOG_DEBUG("V3: " << n3);
                }

                if( pointInTriangle( node.coords[0], node.coords[1], node.coords[2],
                                        n1.coords, n2.coords, n3.coords, debug ) )
                {
                    triangleIntersectionFound = true; 
                    if( debug )
                        LOG_DEBUG("Skipping");
                    continue;
                }

                bool intersectionCheckRequired = (
                    ( n1.isBorder() && n2.isBorder() && n3.isBorder() )
                    || !triangleIntersectionFound
                );
                
                if( debug )
                    LOG_DEBUG("Intersection check: " << intersectionCheckRequired);

                float tmpCoords[3];
                tmpCoords[0] = coords[0];
                tmpCoords[1] = coords[1];
                tmpCoords[2] = coords[2];
                float nodeCoords[3];
                nodeCoords[0] = node.coords.x;
                nodeCoords[1] = node.coords.y;
                nodeCoords[2] = node.coords.z;
                if( intersectionCheckRequired
                        && vectorIntersectsTriangle( n1.coords, n2.coords, n3.coords,
                                                    nodeCoords, direction, dist, tmpCoords, debug)
                )
                {
                    if( debug )
                        LOG_DEBUG("Intersects");
                    if ( n1.isBorder() && n2.isBorder() && n3.isBorder() )
                    {
                        // Tell that point found is border,
                        // coords are already set by vectorIntersectsTriangle()
                        coords[0] = tmpCoords[0];
                        coords[1] = tmpCoords[1];
                        coords[2] = tmpCoords[2];
                        *innerPoint = false;
                        if( debug )
                        {
                            LOG_DEBUG("Intersection node: " << coords[0] << " " << coords[1] << " " << coords[2] );
                            LOG_DEBUG("Expanding scan done");
                        }
                        //return curTetr;
                        updateCharactCache(node, dx, dy, dz, -1);
                        return curTetr.number;
                    }
                    else
                    {
                        triangleIntersectionFound = true;
                    }
                }
            }

            checkedWithNeigh[ tetrsToCheck[i] ] = false;

            // Check if this tetr is still inside sphere of radius R
            // If we have already found at least one tetr in the sphere - skip check
            if(!last_inside_R)
            {
                if( debug )
                    LOG_DEBUG("Checking if we are inside R2 sphere");
                // For all verticles of current tetr
                for(int j = 0; j < 4; j++)
                {
                    CalcNode& vertJ = getNode( curTetr.verts[j] );
                    // Skip base node. Otherwise we'll get false positive insideR for the 1st and 2nd layers
                    if( vertJ.number == node.number )
                        continue;

                    local_x = vertJ.coords[0];
                    local_y = vertJ.coords[1];
                    local_z = vertJ.coords[2];
                    // If its distance smaller than R
                    if( vectorSquareNorm( node.coords[0] - local_x,
                            node.coords[1] - local_y, node.coords[2] - local_z) < R2 )
                    {
                            last_inside_R = true;
                    }
                    // FIXME In theory we whould check if sphere and tetr intersect.
                    // Current check - if at least one vert is in sphere.
                    // It can be wrong on turbo bad tetrs. It fails if
                    // sphere goes 'through' tetr to next layer but does not include its verticles.
                }
                if( debug )
                    LOG_DEBUG("Check result: " << last_inside_R);
            }
        }

        // If current layer is not in sphere - there is no need to create next layer - just return NULL
        if( !inside_R || ( node.isBorder() && !triangleIntersectionFound ) )
        {
            coords[0] = x;    coords[1] = y;    coords[2] = z;
            *innerPoint = false;
            if( debug )
                LOG_DEBUG("Point not found");
            updateCharactCache(node, dx, dy, dz, -1);
            return -1;
        }

        // If not found in current tetrs - create new list for checking
        int checkedTetrNum;
        bool isCompletelyChecked;
        int nextTetrNum;

        tetrsToCheck.clear();

        for( itr = checkedWithNeigh.begin(); itr != checkedWithNeigh.end(); ++itr )
        {
            checkedTetrNum = itr->first;
            isCompletelyChecked = itr->second;
            if( isCompletelyChecked )
                continue;

            TetrFirstOrder& checkedTetr = getTetr(checkedTetrNum);
            for(int j = 0; j < 4; j++)
            {
                CalcNode& vertJ = getNode( checkedTetr.verts[j] );
                vector<int>& elements = getVolumeElementsForNode(vertJ.number);
                for(unsigned k = 0; k < elements.size(); k++)
                {
                    nextTetrNum = elements[k];
                    if( checkedWithNeigh.find(nextTetrNum) == checkedWithNeigh.end() )
                        tetrsToCheck.push_back(nextTetrNum);
                }
            }
            checkedWithNeigh[checkedTetrNum] = true;
        }
    }

    coords[0] = x;    coords[1] = y;    coords[2] = z;
    *innerPoint = false;
    if( debug )
        LOG_DEBUG("Point not found");
    
    updateCharactCache(node, dx, dy, dz, -1);
    return -1;
}

void TetrMeshFirstOrder::findBorderNodeNormal(const CalcNode& _node, float* x, float* y, float* z, bool debug)
{
    assert_true(_node.isBorder() );
    int nodeNumber;
    // Virt nodes
    if(_node.getCustomFlag(CalcNode::FLAG_1)) {
        assert_lt(_node.number, 0);
        double pt[3] = {_node.coords.x, _node.coords.y, _node.coords.z};
        double pos[3];
        CalcNode* pNode;
        struct kdres *presults = kd_nearest(kdtree, pt);
        pNode = (CalcNode*) kd_res_item(presults, pos);
        kd_res_free( presults );
        nodeNumber = pNode->number;
        // Reasonable WA. Experiment shows it is used for ~ N * 10^-4 nodes
        if(!pNode->isBorder()) {
            kdres *presults = kd_nearest_range(kdtree, pt, getMaxEdge());
            while (!kd_res_end(presults)) {
                /* get the data and position of the current result item */
                pNode = (CalcNode*) kd_res_item(presults, pos);
                if(pNode->isBorder()) {
                    nodeNumber = pNode->number;
                    break;
                }
                /* go to the next entry */
                kd_res_next(presults);
            }
            kd_res_free( presults );
        }
    // real nodes
    } else {
        nodeNumber = _node.number;
    }
    
    CalcNode& node = getNode(nodeNumber);
    
    assert_ge(node.number, 0);
    
    assert_true(node.isBorder() );
    
    float final_normal[3];
    final_normal[0] = 0;
    final_normal[1] = 0;
    final_normal[2] = 0;

    float cur_normal[3];

    vector<int>& borderElements = getBorderElementsForNode(node.number);
    int count = borderElements.size();
    int triNumber;

    float h = mesh_min_h * 0.5;

    if( count <= 0 )
    {
        LOG_WARN("Border node does not belong to any face");
        LOG_WARN("Can not find normal for the node " << node);
        //node.setNormalError();
    }

    for(int i = 0; i < count; i++)
    {
        triNumber = borderElements[i];
        find_border_elem_normal( triNumber, &cur_normal[0], &cur_normal[1], &cur_normal[2] );

        final_normal[0] += cur_normal[0];
        final_normal[1] += cur_normal[1];
        final_normal[2] += cur_normal[2];
        if( debug )
        {
            LOG_TRACE("Element " << triNumber);
            LOG_TRACE("Vert #1: " << getNode( getTriangle(triNumber).verts[0] ));
            LOG_TRACE("Vert #2: " << getNode( getTriangle(triNumber).verts[1] ));
            LOG_TRACE("Vert #3: " << getNode( getTriangle(triNumber).verts[2] ));
            LOG_TRACE("Normal: " << cur_normal[0] << " " << cur_normal[1] << " " << cur_normal[2]);
        }
    }

    final_normal[0] /= count;
    final_normal[1] /= count;
    final_normal[2] /= count;

    float scale = 1 / vectorNorm( final_normal[0], final_normal[1], final_normal[2] );
    final_normal[0] *= scale;
    final_normal[1] *= scale;
    final_normal[2] *= scale;

    *x = final_normal[0];
    *y = final_normal[1];
    *z = final_normal[2];

    if( ! isInnerPoint(node, - h * final_normal[0], - h * final_normal[1], - h * final_normal[2], debug))
    {
        if( !debug )
        {
            LOG_TRACE("Sharp border - can not create normal for node: " << node );
            LOG_TRACE("Normal: " << final_normal[0] << " " << final_normal[1] << " " << final_normal[2]);
            LOG_TRACE("Re-running search with debug on");
            findBorderNodeNormal(node, x, y, z, true);
        }
    }
}

float TetrMeshFirstOrder::get_solid_angle(int node_index, int tetr_index)
{
    TetrFirstOrder& tetr = getTetr(tetr_index);
    CalcNode& node = getNode(node_index);
    // Node should belong to Tetr
    assert_true(( tetr.verts[0] == node_index )
            || ( tetr.verts[1] == node_index )
            || ( tetr.verts[2] == node_index )
            || ( tetr.verts[3] == node_index ) );
    /*{
        LOG_WARN("TEST: " << node_index << " " << tetr.verts[0] << " " << tetr.verts[1] << " " << tetr.verts[2] << " " << tetr.verts[3]);
        LOG_WARN("TEST: " << node);
        LOG_WARN("TEST: " << getNode( tetr.verts[0] ));
        LOG_WARN("TEST: " << getNode( tetr.verts[1] ));
        LOG_WARN("TEST: " << getNode( tetr.verts[2] ));
        LOG_WARN("TEST: " << getNode( tetr.verts[3] ));
        return -1;
    }*/

    int verts[3];
    int count = 0;

    for(int i = 0; i < 4; i++)
        if(tetr.verts[i] != node_index) {
            verts[count] = tetr.verts[i];
            count++;
        }

    // We are to find 3 other verticles to form the face we are looking at
    assert_eq(count, 3 );

    CalcNode& v0 = getNode( verts[0] );
    CalcNode& v1 = getNode( verts[1] );
    CalcNode& v2 = getNode( verts[2] );

    return solidAngle(
            v0.coords[0] - node.coords[0],
            v0.coords[1] - node.coords[1],
            v0.coords[2] - node.coords[2],
            v1.coords[0] - node.coords[0],
            v1.coords[1] - node.coords[1],
            v1.coords[2] - node.coords[2],
            v2.coords[0] - node.coords[0],
            v2.coords[1] - node.coords[1],
            v2.coords[2] - node.coords[2]
    );
};

void TetrMeshFirstOrder::find_border_elem_normal(int border_element_index,
                                                        float* x, float* y, float* z)
{
    TriangleFirstOrder& tri = getTriangle(border_element_index);
    findTriangleFaceNormal( getNode( tri.verts[0] ).coords,
                            getNode( tri.verts[1] ).coords,
                            getNode( tri.verts[2] ).coords,
                            x, y, z );
};

void TetrMeshFirstOrder::calcMinH()
{
    //assert_gt(tetrsNumber, 0 );
    if( tetrsNumber == 0 )
        return;
    float min_h = tetr_h(tetrsMap.begin()->first);
    assert_gt(min_h, 0 );

    float h;
    // Go through tetrahedrons
    //for(int i = 0; i < tetrsNumber; i++) {
    for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr ) {
        int i = itr->first;
        TetrFirstOrder& tetr = getTetr(i);
        if ( ( !getNode( tetr.verts[0] ).isUsed() )
            || ( !getNode( tetr.verts[1] ).isUsed() )
            || ( !getNode( tetr.verts[2] ).isUsed() )
            || ( !getNode( tetr.verts[3] ).isUsed() ) )
            continue;

        // Get current h
        h = tetr_h(i);
        assert_gt(h, 0 );
        // Otherwise - just find minimum
        if(h < min_h) { min_h = h; }
    }
    mesh_min_h = min_h;
    if( mesh_min_h < EQUALITY_TOLERANCE )
    {
        LOG_WARN("Mesh minH is too small: minH " << mesh_min_h << ", FP tolerance: " << EQUALITY_TOLERANCE);
        LOG_WARN("Fixing it automatically, but it can cause numerous intersting issues");
        mesh_min_h = 10 * EQUALITY_TOLERANCE;
    }
};

void TetrMeshFirstOrder::calcMaxH()
{
    //assert_gt(tetrsNumber, 0 );
    if( tetrsNumber == 0 )
        return;
    float max_h = tetr_h(tetrsMap.begin()->first);
    assert_gt(max_h, 0 );

    float h;
    // Go through tetrahedrons
    //for(int i = 0; i < tetrsNumber; i++) {
    for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr ) {
        int i = itr->first;
        TetrFirstOrder& tetr = getTetr(i);
        if ( ( !getNode( tetr.verts[0] ).isUsed() )
            || ( !getNode( tetr.verts[1] ).isUsed() )
            || ( !getNode( tetr.verts[2] ).isUsed() )
            || ( !getNode( tetr.verts[3] ).isUsed() ) )
            continue;

        // Get current h
        h = tetr_h(i);
        assert_gt(h, 0 );
        // Otherwise - just find minimum
        if(h > max_h) { max_h = h; }
    }
    mesh_max_h = max_h;
};

void TetrMeshFirstOrder::calcMaxEdge()
{
    if( tetrsNumber == 0 )
        return;
    float max_edge = - numeric_limits<float>::infinity();
    
    std::pair <int, int> combinations[6];
    combinations[0] = make_pair(0, 1);
    combinations[1] = make_pair(0, 2);
    combinations[2] = make_pair(0, 3);
    combinations[3] = make_pair(1, 2);
    combinations[4] = make_pair(1, 3);
    combinations[5] = make_pair(2, 3);
    
    float edge;
    // Go through tetrahedrons
    for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr ) {
        int i = itr->first;
        TetrFirstOrder& tetr = getTetr(i);
        
        for (int j = 0; j < 6; j++) {
            CalcNode& n1 = getNode(tetr.verts[ combinations[j].first ]);
            CalcNode& n2 = getNode(tetr.verts[ combinations[j].second ]);
            
            // Calc current edge
            edge = vectorNorm(n1.coords.x - n2.coords.x, n1.coords.y - n2.coords.y, n1.coords.z - n2.coords.z);
            assert_gt(edge, 0 );
            // Otherwise - just find minimum
            if(edge > max_edge) { max_edge = edge; }
        }
    }
    
    mesh_max_edge = max_edge;
};

void TetrMeshFirstOrder::calcAvgH()
{
    //assert_gt(tetrsNumber, 0 );
    if( tetrsNumber == 0 )
        return;
    float avg_h = 0;

    float h;
    // Go through tetrahedrons
    //for(int i = 0; i < tetrsNumber; i++) {
    for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr ) {
        int i = itr->first;
        TetrFirstOrder& tetr = getTetr(i);
        if ( ( !getNode( tetr.verts[0] ).isUsed() )
            || ( !getNode( tetr.verts[1] ).isUsed() )
            || ( !getNode( tetr.verts[2] ).isUsed() )
            || ( !getNode( tetr.verts[3] ).isUsed() ) )
            continue;

        // Get current h
        h = tetr_h(i);
        assert_gt(h, 0 );
        // Otherwise - just find minimum
        avg_h += h;
    }
    mesh_avg_h = avg_h / tetrsNumber;
};

float TetrMeshFirstOrder::tetr_h(int i)
{
    TetrFirstOrder& tetr = getTetr(i);
    return tetrHeight( getNode(tetr.verts[0]).coords, getNode(tetr.verts[1]).coords,
                        getNode(tetr.verts[2]).coords, getNode(tetr.verts[3]).coords );
};

void TetrMeshFirstOrder::doNextPartStep(float tau, int stage)
{
    defaultNextPartStep(tau, stage);
};

void TetrMeshFirstOrder::logMeshStats()
{
    if( std::isinf( getMaxH() ) )
    {
        LOG_DEBUG("Mesh is empty");
        return;
    }
    
    int borderCount = 0;
    int volumeCount = 0;
    for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr )
    {
        int i = itr->first;
        CalcNode& node = getNode(i);
        if( node.isBorder() )
            borderCount++;
        else
            volumeCount++;
    }

    LOG_DEBUG("Number of nodes: " << nodesNumber);
    LOG_DEBUG("Border / Volume: " << (float)borderCount / (float)volumeCount 
            << " (" << borderCount << "/" << volumeCount << ")");
    LOG_DEBUG("Number of tetrs: " << tetrsNumber);

    LOG_DEBUG("Mesh outline:" << outline);
    LOG_DEBUG("Mesh expanded outline:" << expandedOutline);

    LOG_DEBUG("Mesh quality:");
    LOG_DEBUG("Max H = " << getMaxH());
    LOG_DEBUG("Min H = " << getMinH());
    LOG_DEBUG("Avg H = " << getAvgH());
    LOG_DEBUG("Max Edge = " << getMaxEdge());
};

void TetrMeshFirstOrder::checkTopology(float tau)
{
    LOG_DEBUG("Checking mesh topology");
    
    LOG_DEBUG("Creating spatial index");
    initSpatialIndex();
    LOG_DEBUG("Creating spatial index done");

    if( std::isinf( getMaxH() ) )
    {
        LOG_DEBUG("Mesh is empty");
        return;
    }

    auto& e = Engine::getInstance();
    GCMDispatcher* d = e.getDispatcher();
    int workers = e.getNumberOfWorkers();
    //int rank = e.getRank();

    // FIXME@avasyukov - rethink it
    if( d->getOutline(0) == NULL )
    {
        LOG_WARN("FIXME: we need getOutline() back!");
        return;
    }

    float maxLambda = getMaxEigenvalue();
    for( int i = 0; i < 3; i++ )
    {
        areaOfInterest.min_coords[i] = outline.min_coords[i] - 2 * maxLambda * tau;
        areaOfInterest.max_coords[i] = outline.max_coords[i] + 2 * maxLambda * tau;
    }

    // FIXME@avasyukov - rethink it
    if( std::isinf( syncedArea.minX ) )
    {
        memcpy( syncedArea.min_coords, outline.min_coords, 3 * sizeof(float) );
        memcpy( syncedArea.max_coords, outline.max_coords, 3 * sizeof(float) );
    }

    /*
    memcpy( syncedArea.min_coords, expandedOutline.min_coords, 3 * sizeof(float) );
    memcpy( syncedArea.max_coords, expandedOutline.max_coords, 3 * sizeof(float) );

    for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr )
    {
        int i = itr->first;
        CalcNode& node = getNode(i);
        if( !node.isBorder() )
            continue;

        for( int j = 0; j < 3; j++ )
        {
            int targetCoord = (0 + j) % 3;
            int coord1 = (1 + j) % 3;
            int coord2 = (2 + j) % 3;

            if( ( node.coords[coord1] < outline.max_coords[coord1] )
                    && ( node.coords[coord1] > outline.min_coords[coord1] )
                    && ( node.coords[coord2] < outline.max_coords[coord2] )
                    && ( node.coords[coord2] > outline.min_coords[coord2] )
                    && ( node.coords[targetCoord] > outline.max_coords[targetCoord] )
                    && ( node.coords[targetCoord] < syncedArea.max_coords[targetCoord] )
                )
            {
                LOG_DEBUG("First pass: synced area is adjusted by node: "<< *node);
                syncedArea.max_coords[targetCoord] = node.coords[targetCoord];
            }

            if( ( node.coords[coord1] < outline.max_coords[coord1] )
                    && ( node.coords[coord1] > outline.min_coords[coord1] )
                    && ( node.coords[coord2] < outline.max_coords[coord2] )
                    && ( node.coords[coord2] > outline.min_coords[coord2] )
                    && ( node.coords[targetCoord] < outline.min_coords[targetCoord] )
                    && ( node.coords[targetCoord] > syncedArea.min_coords[targetCoord] )
                )
            {
                LOG_DEBUG("First pass: synced area is adjusted by node: "<< *node);
                syncedArea.min_coords[targetCoord] = node.coords[targetCoord];
            }
        }
    }

    LOG_DEBUG("First pass done. Synced area: " << syncedArea);

    for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr )
    {
        int i = itr->first;
        CalcNode& node = getNode(i);
        if( !node.isBorder() )
            continue;

        if( syncedArea.isInAABB(node) )
        {
            int indMin = -1;
            int indMax = -1;
            float distMin = numeric_limits<float>::infinity();
            float distMax = numeric_limits<float>::infinity();
            for( int j = 0; j < 3; j++ )
            {
                if( fabs(node.coords[j] - syncedArea.min_coords[j]) < distMin )
                {
                    distMin = fabs(node.coords[j] - syncedArea.min_coords[j]);
                    indMin = j;
                }
                if( fabs(node.coords[j] - syncedArea.max_coords[j]) < distMax )
                {
                    distMax = fabs(node.coords[j] - syncedArea.max_coords[j]);
                    indMax = j;
                }
            }
            if( distMin < distMax )
                syncedArea.min_coords[indMin] = node.coords[indMin];
            else
                syncedArea.max_coords[indMax] = node.coords[indMax];
        }
    }

    LOG_DEBUG("Second pass done. Synced area: " << syncedArea);
    */

    AABB globalOutline;
    for(int i = 0; i < workers; i++)
    {
        AABB* outline = d->getOutline(i);
        for(int j = 0; j < 3; j++)
        {
            if( outline->min_coords[j] < globalOutline.min_coords[j] )
                globalOutline.min_coords[j] = outline->min_coords[j];
            if( outline->max_coords[j] > globalOutline.max_coords[j] )
                globalOutline.max_coords[j] = outline->max_coords[j];
        }
    }
    for(int j = 0; j < 3; j++)
    {
        if( areaOfInterest.min_coords[j] < globalOutline.min_coords[j] )
            areaOfInterest.min_coords[j] = globalOutline.min_coords[j];
        if( areaOfInterest.max_coords[j] > globalOutline.max_coords[j] )
            areaOfInterest.max_coords[j] = globalOutline.max_coords[j];
    }

    LOG_DEBUG("Mesh initial area of interest: " << areaOfInterest);
    if( e.getNumberOfWorkers() == 1 )
    {
        // Everything is local anyway
        for( int i = 0; i < 3; i++ )
        {
            areaOfInterest.min_coords[i] = outline.min_coords[i];
            areaOfInterest.max_coords[i] = outline.max_coords[i];
        }
        /*bool intersects = false;
        for(int j = 0; j < e->getNumberOfBodies(); j++)
        {
            Mesh* m = e->getBody(j)->getMeshes();
            if( m->getId() == getId() )
                continue;
            AABB box = m->getOutline();
            if( areaOfInterest.intersects(&box) )
                intersects = true;
        }
        if( !intersects )
        {
            for( int i = 0; i < 3; i++ )
            {
                areaOfInterest.min_coords[i] = outline.min_coords[i];
                areaOfInterest.max_coords[i] = outline.max_coords[i];
            }
        }*/
    }
    else
    {
        bool intersects = false;
        for(int j = 0; j < e.getNumberOfWorkers(); j++)
        {
            if( j == e.getRank() )
                continue;
            AABB box = *(e.getDispatcher()->getOutline(j));
            if( areaOfInterest.intersects(&box) )
                intersects = true;
        }
        if( !intersects )
        {
            for( int i = 0; i < 3; i++ )
            {
                areaOfInterest.min_coords[i] = outline.min_coords[i];
                areaOfInterest.max_coords[i] = outline.max_coords[i];
            }
        }
    }

    LOG_DEBUG("Mesh outline: " << outline);
    LOG_DEBUG("Mesh expanded outline: " << expandedOutline);
    LOG_DEBUG("Mesh area of interest: " << areaOfInterest);
    LOG_DEBUG("Mesh synced area: " << syncedArea);
}

float TetrMeshFirstOrder::getRecommendedTimeStep()
{
    return getAvgH() / getMaxEigenvalue();
};

float TetrMeshFirstOrder::getMinH()
{
    if( std::isinf( mesh_min_h ) )
        calcMinH();
    return mesh_min_h;
}

float TetrMeshFirstOrder::getMaxH()
{
    if( std::isinf( mesh_max_h ) )
        calcMaxH();
    return mesh_max_h;
}

float TetrMeshFirstOrder::getMaxEdge()
{
    if( std::isinf( mesh_max_edge ) )
        calcMaxEdge();
    return mesh_max_edge;
}

float TetrMeshFirstOrder::getAvgH()
{
    if( std::isinf( mesh_avg_h ) )
        calcAvgH();
    return mesh_avg_h;
}

bool TetrMeshFirstOrder::checkCharactCache(const CalcNode& node, float dx, float dy, float dz, int& tetrNum)
{
    unordered_map<int, std::unordered_set<int>>::const_iterator itr = charactCache.find(node.number);
    if( itr == charactCache.cend() )
        return false;
    std::unordered_set<int>::const_iterator candidate = (itr->second).cbegin();
    std::unordered_set<int>::const_iterator end = (itr->second).cend();
    for(; candidate != end; ++candidate) {
        if(*candidate < 0)
            continue;

        TetrFirstOrder &curTetr = getTetr(*candidate);
        bool check = pointInTetr(
                node.coords[0] + dx, node.coords[1] + dy, node.coords[2] + dz,
                getNode(curTetr.verts[0]).coords,
                getNode(curTetr.verts[1]).coords,
                getNode(curTetr.verts[2]).coords,
                getNode(curTetr.verts[3]).coords,
                false);
        if(check) {
            tetrNum = *candidate;
            return check;
        }
    }
    return false;
}

void TetrMeshFirstOrder::updateCharactCache(const CalcNode& node, float dx, float dy, float dz, int tetrNum)
{
    if( !charactCacheAvailable() )
        return;
    charactCache[node.number].insert(tetrNum);
}

int TetrMeshFirstOrder::getCharactCacheIndex(const CalcNode& node, float dx, float dy, float dz)
{
    float fdx = fabs(dx);
    float fdy = fabs(dy);
    float fdz = fabs(dz);
    
    // FIXME ASAP: calling m.createAi() on each cache index access is horrible for performance
    RheologyMatrixPtr m = node.getRheologyMatrix();

    short sign = -1;
    short direction = -1;
    if( fdx > fdy )
        if( fdx > fdz )
        {
            direction = 1;
            m->decomposeX(node);
            sign = (dx > 0 ? 2 : 1);
        }
        else
        {
            direction = 3;
            m->decomposeZ(node);
            sign = (dz > 0 ? 2 : 1);
        }
    else
        if( fdy > fdz )
        {
            direction = 2;
            m->decomposeY(node);
            sign = (dy > 0 ? 2 : 1);
        }
        else
        {
            direction = 3;
            m->decomposeZ(node);
            sign = (dz > 0 ? 2 : 1);
        }

    float l2 = dx*dx + dy*dy + dz*dz;
    float tau = Engine::getInstance().getTimeStep();


    float maxV = m->getMaxEigenvalue();
    float minV = m->getMinEigenvalue();

    short scale = -1;
    if( fabs(l2 - tau*tau*maxV*maxV) < l2 * EQUALITY_TOLERANCE )
        scale = 3;
    else if( fabs(l2 - tau*tau*minV*minV) < l2 * EQUALITY_TOLERANCE )
        scale = 1;
    else
        scale = 2;

    assert_gt(direction, 0);
    assert_lt(direction, 4);
    assert_gt(scale, 0);
    assert_lt(scale, 4);
    assert_true(sign == 1 || sign == 2);

    return (direction - 1)*6 + (sign-1)*3 + (scale-1);
}

bool TetrMeshFirstOrder::charactCacheAvailable()
{
    return true;
    /*return ( nodesNumber > 0
            && Engine::getInstance().getTimeStep() > 0
            && getNodeByLocalIndex(0).getMaterialId() >= 0
            && getNodeByLocalIndex(0).getMaterialId() < Engine::getInstance().getNumberOfMaterials() );*/
}

bool TetrMeshFirstOrder::interpolateNode(CalcNode& origin, float dx, float dy, float dz, bool debug,
                                                CalcNode& targetNode, bool& isInnerPoint)
{
    int tetrInd = findOwnerTetr( origin, dx, dy, dz, debug,
                                    targetNode.coords, &isInnerPoint );

    if( tetrInd == -1 )
        return false;

    TetrFirstOrder& tmp_tetr = getTetr( tetrInd );
    interpolator->interpolate( targetNode,
            getNode( tmp_tetr.verts[0] ), getNode( tmp_tetr.verts[1] ),
            getNode( tmp_tetr.verts[2] ), getNode( tmp_tetr.verts[3] ) );
    return true;
}

// FIXME_ASAP: rewrite it
bool TetrMeshFirstOrder::interpolateNode(CalcNode& node)
{
    for (int i = 0; i < getTetrsNumber(); i++)
    {
        TetrFirstOrder& t = getTetrByLocalIndex(i);
        if ( pointInTetr(node.coords.x, node.coords.y, node.coords.z,
                getNode(t.verts[0]).coords, getNode(t.verts[1]).coords,
                getNode(t.verts[2]).coords, getNode(t.verts[3]).coords, false) )
        {
            interpolator->interpolate( node,
                    getNode( t.verts[0] ), getNode( t.verts[1] ),
                    getNode( t.verts[2] ), getNode( t.verts[3] ) );
            return true;
        }
    }

    return false;
}

bool TetrMeshFirstOrder::interpolateBorderNode(real x, real y, real z, 
                                real dx, real dy, real dz, CalcNode& node)
{
    real start[3];
    start[0] = x;
    start[1] = y;
    start[2] = z;
    real direction[3];
    real length;
    length = vectorNorm(dx, dy, dz);
    direction[0] = dx / length;
    direction[1] = dy / length;
    direction[2] = dz / length;
    
    vector<int> targetFaces;
    double pt[3] = { x + dx * 0.5, y + dy * 0.5, z + dz * 0.5 };
    double pos[3];
    CalcNode* pNode;
    struct kdres *presults = kd_nearest_range(kdtree, pt, length * 0.5 + getMaxH());
    while (!kd_res_end(presults)) {
        /* get the data and position of the current result item */
        pNode = (CalcNode*) kd_res_item(presults, pos);
        vector<int> cf = getBorderElementsForNode(pNode->number);
        targetFaces.insert(targetFaces.end(), cf.begin(), cf.end());
        /* go to the next entry */
        kd_res_next(presults);
    }
    kd_res_free( presults );
    
    std::sort(targetFaces.begin(), targetFaces.end());
    
    //bool tmpRes = false;
    //CalcNode tmpNode;
    for (unsigned int i = 0; i < targetFaces.size(); i++)
    {
        TriangleFirstOrder& face = getTriangle(targetFaces[i]);
        CalcNode& n1 = getNode(face.verts[0]);
        CalcNode& n2 = getNode(face.verts[1]);
        CalcNode& n3 = getNode(face.verts[2]);
        // FIXME_ASAP - ugly WA 
        node = n1;
        real xmin = n1.coords[0], ymin = n1.coords[1], zmin = n1.coords[2], xmax = n1.coords[0], ymax = n1.coords[1], zmax = n1.coords[2];
		if (n2.coords[0] < xmin) xmin = n2.coords[0];
        if (n2.coords[0] > xmax) xmax = n2.coords[0];
        if (n3.coords[0] < xmin) xmin = n3.coords[0];
        if (n3.coords[0] > xmax) xmax = n3.coords[0];
        if (n2.coords[1] < ymin) ymin = n2.coords[1];
        if (n2.coords[1] > ymax) ymax = n2.coords[1];
        if (n3.coords[1] < ymin) ymin = n3.coords[1];
        if (n3.coords[1] > ymax) ymax = n3.coords[1];
        if (n2.coords[2] < zmin) zmin = n2.coords[2];
        if (n2.coords[2] > zmax) zmax = n2.coords[2];
        if (n3.coords[2] < zmin) zmin = n3.coords[2];
        if (n3.coords[2] > zmax) zmax = n3.coords[2];
		xmin -= length;
        ymin -= length;
        zmin -= length;
        xmax += length;
        ymax += length;
        zmax += length;

	if (x < xmin || x > xmax || y < ymin || y > ymax || z < zmin || z > zmax)
		continue;
        
        if(vectorIntersectsTriangle( n1.coords, n2.coords, n3.coords,
                                     start, direction, length, node.coords, false))
        {
            double d = vectorNorm(x - node.coords.x, y - node.coords.y, z - node.coords.z);
            if( (node.coords.x - x) * dx + (node.coords.y - y) * dy + (node.coords.z - z) * dz < 0 
               || d > length )
            {
                LOG_DEBUG("Proposed point: " << node);
                THROW_BAD_MESH("interpolateBorderNode did smth really bad");
            }
            
            interpolateTriangle( n1.coords, n2.coords, n3.coords, node.coords,
                                n1.values, n2.values, n3.values, node.values, 9);
            node.setRho((getNode(face.verts[0])).getRho());
            node.setMaterialId((getNode(face.verts[0])).getMaterialId());

            //tmpRes = true;
            //break;
            //node = tmpNode;
            node.number = -1;
            return true;
        }
    }
    return false;
    
    /*
    for (int i = 0; i < faceNumber; i++)
    {
        TriangleFirstOrder& face = getTriangle(i);
        CalcNode& n1 = getNode(face.verts[0]);
        CalcNode& n2 = getNode(face.verts[1]);
        CalcNode& n3 = getNode(face.verts[2]);
        // FIXME_ASAP - ugly WA 
        node = n1;
        real xmin = n1.coords[0], ymin = n1.coords[1], zmin = n1.coords[2], xmax = n1.coords[0], ymax = n1.coords[1], zmax = n1.coords[2];
		if (n2.coords[0] < xmin) xmin = n2.coords[0];
        if (n2.coords[0] > xmax) xmax = n2.coords[0];
        if (n3.coords[0] < xmin) xmin = n3.coords[0];
        if (n3.coords[0] > xmax) xmax = n3.coords[0];
        if (n2.coords[1] < ymin) ymin = n2.coords[1];
        if (n2.coords[1] > ymax) ymax = n2.coords[1];
        if (n3.coords[1] < ymin) ymin = n3.coords[1];
        if (n3.coords[1] > ymax) ymax = n3.coords[1];
        if (n2.coords[2] < zmin) zmin = n2.coords[2];
        if (n2.coords[2] > zmax) zmax = n2.coords[2];
        if (n3.coords[2] < zmin) zmin = n3.coords[2];
        if (n3.coords[2] > zmax) zmax = n3.coords[2];
		xmin -= length;
        ymin -= length;
        zmin -= length;
        xmax += length;
        ymax += length;
        zmax += length;

	if (x < xmin || x > xmax || y < ymin || y > ymax || z < zmin || z > zmax)
		continue;
        
        if(vectorIntersectsTriangle( n1.coords, n2.coords, n3.coords,
                                     start, direction, length, node.coords, false))
        {
                interpolateTriangle( n1.coords, n2.coords, n3.coords, node.coords,
                                     n1.values, n2.values, n3.values, node.values, 9);
                node.setRho((getNode(face.verts[0])).getRho());
                node.setMaterialId((getNode(face.verts[0])).getMaterialId());

                double d = vectorNorm(node.coords.x - tmpNode.coords.x, node.coords.y - tmpNode.coords.y, node.coords.z - tmpNode.coords.z);
                if(!tmpRes || d > length * 0.1) {
                    LOG_DEBUG("KD_RES: " << tmpRes);
                    LOG_DEBUG("NODE: " << x << " " << y << " " << z);
                    LOG_DEBUG("MOVE: " << dx << " " << dy << " " << dz);
                    LOG_DEBUG("R_ORIG: " << node);
                    LOG_DEBUG("R_KD: " << tmpNode);
                }
                return true;
        }
    }

    assert_false(tmpRes);
    return false;
     */
}

vector<int>& TetrMeshFirstOrder::getVolumeElementsForNode(int index)
{
    // Get local index
    unsigned int localIndex = getNodeLocalIndex(index);
    // Resize on demand
    if( localIndex >= volumeElements.size() )
        volumeElements.resize(localIndex + 1);
    return volumeElements[localIndex];
}

vector<int>& TetrMeshFirstOrder::getBorderElementsForNode(int index)
{
    // Get local index
    unsigned int localIndex = getNodeLocalIndex(index);
    // Resize on demand
    if( localIndex >= borderElements.size() )
        borderElements.resize(localIndex + 1);
    return borderElements[localIndex];
}


const SnapshotWriter& TetrMeshFirstOrder::getSnaphotter() const
{
    return VTKSnapshotWriter::getInstance();
}

const SnapshotWriter& TetrMeshFirstOrder::getDumper() const
{
    return getSnaphotter();
}
