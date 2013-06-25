#include "TetrMeshFirstOrder.h"

gcm::TetrMeshFirstOrder::TetrMeshFirstOrder() {
	nodes = NULL;
	new_nodes = NULL;
	tetrs1 = NULL;
	border1 = NULL;
	nodesNumber = 0;
	nodesStorageSize = 0;
	tetrsNumber = 0;
	faceNumber = 0;
	mesh_min_h = numeric_limits<float>::infinity();
	mesh_max_h = numeric_limits<float>::infinity();
	mesh_avg_h = numeric_limits<float>::infinity();
	triSizeInBytes = sizeof(TriangleFirstOrder);
	tetrSizeInBytes = sizeof(TetrFirstOrder);
	numericalMethodOrder = 1;
	INIT_LOGGER("gcm.TetrMeshFirstOrder");
	LOG_DEBUG("Creating mesh");
}

gcm::TetrMeshFirstOrder::~TetrMeshFirstOrder() {
	LOG_DEBUG("Destroying mesh '" << getId() << "'");
	if( nodes != NULL )
		delete[] nodes;
		//nodeFactory->destroyNodes(nodes);
	if( new_nodes != NULL )
		delete[] new_nodes;
		//nodeFactory->destroyNodes(new_nodes);
	if( tetrs1 != NULL )
		delete[] tetrs1;
	if( border1 != NULL )
		delete[] border1;
	LOG_DEBUG("Mesh destroyed");
}

int gcm::TetrMeshFirstOrder::getNodesNumber() {
	return nodesNumber;
}

int gcm::TetrMeshFirstOrder::getTetrsNumber() {
	return tetrsNumber;
}

int gcm::TetrMeshFirstOrder::getTriangleNumber() {
	return faceNumber;
}

ElasticNode* gcm::TetrMeshFirstOrder::getNode(int index) {
	assert( index >= 0 );
	map<int, int>::const_iterator itr;
	itr = nodesMap.find(index);
	return ( itr != nodesMap.end() ? nodes + itr->second : NULL );
	//if(  == nodesMap.end() )
	//	return NULL;
	//return nodes+nodesMap[index];
}

ElasticNode* gcm::TetrMeshFirstOrder::getNewNode(int index) {
	assert( index >= 0 );
	map<int, int>::const_iterator itr;
	itr = nodesMap.find(index);
	return ( itr != nodesMap.end() ? new_nodes + itr->second : NULL );
	//if( nodesMap.find(index) == nodesMap.end() )
	//	return NULL;
	//return new_nodes+nodesMap[index];
}

ElasticNode* gcm::TetrMeshFirstOrder::getNodeByLocalIndex(int index) {
	assert( index >= 0 );
	return nodes + index;
}

int gcm::TetrMeshFirstOrder::getNodeLocalIndex(int index) {
	assert( index >= 0 );
	map<int, int>::const_iterator itr;
	itr = nodesMap.find(index);
	return ( itr != nodesMap.end() ? itr->second : -1 );
}

TetrFirstOrder* gcm::TetrMeshFirstOrder::getTetr(int index) {
	assert( index >= 0 );
	map<int, int>::const_iterator itr;
	itr = tetrsMap.find(index);
	return ( itr != tetrsMap.end() ? 
		(TetrFirstOrder*)((char*)tetrs1 + itr->second * tetrSizeInBytes) : NULL );
	//if( tetrsMap.find(index) == tetrsMap.end() )
	//	return NULL;
	//return (TetrFirstOrder*)((char*)tetrs1 + tetrsMap[index] * tetrSizeInBytes);
}

TetrFirstOrder* gcm::TetrMeshFirstOrder::getTetrByLocalIndex(int index) {
	assert( index >= 0 );
	return (TetrFirstOrder*)((char*)tetrs1 + index * tetrSizeInBytes);
}

TriangleFirstOrder* gcm::TetrMeshFirstOrder::getTriangle(int index) {
	assert( index >= 0 );
	return (TriangleFirstOrder*)((char*)border1 + index * triSizeInBytes);
}

void gcm::TetrMeshFirstOrder::createNodes(int number) {
	assert( nodeFactory != NULL );
	// FIXME currently it's not possible to add more nodes
	if( nodesStorageSize > 0 ) {
		nodeFactory->destroyNodes(nodes);
		nodeFactory->destroyNodes(new_nodes);
	}
	// FIXME think how to avoid typecasts or make them safe
	nodes = static_cast<ElasticNode*>(nodeFactory->makeNodes(number*STORAGE_OVERCOMMIT_RATIO));
	new_nodes = static_cast<ElasticNode*>(nodeFactory->makeNodes(number*STORAGE_OVERCOMMIT_RATIO));
	nodesNumber = 0;
	nodesStorageSize = number*STORAGE_OVERCOMMIT_RATIO;
}

void gcm::TetrMeshFirstOrder::createTetrs(int number) {
	delete[] tetrs1;
	tetrs1 = new TetrFirstOrder[(int)(number*STORAGE_OVERCOMMIT_RATIO)];
	tetrsNumber = 0;
	tetrsStorageSize = number*STORAGE_OVERCOMMIT_RATIO;
}

void gcm::TetrMeshFirstOrder::createTriangles(int number) {
	delete[] border1;
	border1 = new TriangleFirstOrder[number];
	faceNumber = number;
	faceStorageSize = number;
}

void gcm::TetrMeshFirstOrder::addNode(ElasticNode* node) {
	assert( nodesNumber < nodesStorageSize );
	nodes[nodesNumber] = *node;
	nodesMap[node->number] = nodesNumber;
	nodesNumber++;
}

void gcm::TetrMeshFirstOrder::addTetr(TetrFirstOrder* tetr) {
	assert( tetrsNumber < tetrsStorageSize );
	tetrs1[tetrsNumber] = *tetr;
	tetrsMap[tetr->number] = tetrsNumber;
	tetrsNumber++;
}

/*ElasticNode* gcm::TetrMeshFirstOrder::getNodes() {
	return nodes;
}*/

/*TetrFirstOrder* gcm::TetrMeshFirstOrder::getTetrs() {
	return tetrs1;
}*/

/*TriangleFirstOrder* gcm::TetrMeshFirstOrder::getBorder() {
	return border1;
}*/

void gcm::TetrMeshFirstOrder::copyMesh(TetrMeshFirstOrder* src)
{
	LOG_INFO("Creating mesh using copy");
	int firstOrderNodesNumber = src->getNodesNumber();
	
	createNodes( firstOrderNodesNumber );
	for( int i = 0; i < firstOrderNodesNumber; i++ )
		addNode( src->getNode(i) );
	
	createTetrs(src->getTetrsNumber());
	
	//for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr ) {
	//	int i = itr->first;
	//	addTetr( src->getTetr(i) );
	//}
	for( int i = 0; i < src->getTetrsNumber(); i++ )
	{
		addTetr( src->getTetrByLocalIndex(i) );
	}
}

void gcm::TetrMeshFirstOrder::preProcess() {
	LOG_DEBUG("Preprocessing mesh started.");
	
	initNewNodes();
	create_outline();
	calc_min_h();
	calc_max_h();
	calc_avg_h();
	
	verifyTetrahedraVertices ();
	build_volume_reverse_lookups();

	/*if( body->getEngine()->getRank() == 1 )
	{
		LOG_DEBUG("Nodes map:");
		for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
			LOG_DEBUG("Node " << itr->first << ": index " << itr->second);
		}
		LOG_DEBUG("Tetrs map:");
		for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr ) {
			LOG_DEBUG("Tetr " << itr->first << ": index " << itr->second);
		}
	}*/
	
	//check_unused_nodes();	
	build_border();
	build_surface_reverse_lookups();

	check_numbering();
	// check_outer_normals();
	
	LOG_DEBUG("Preprocessing mesh done.");
	logMeshStats();
}

void gcm::TetrMeshFirstOrder::initNewNodes() {
	ElasticNode* node;
	ElasticNode* newNode;
	//for( int i = 0; i < nodesNumber; i++ ) {
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		node = getNode(i);
		newNode = getNewNode(i);
		memcpy( newNode->coords, node->coords, 3*sizeof(float) );
		memcpy( newNode->values, node->values, GCM_VALUES_SIZE*sizeof(float) );
		// FIXME - we can not be sure that nodes are Elastic
		memcpy( newNode->elasticRheologyProperties, node->elasticRheologyProperties, 3*sizeof(float) );
	}
}

void gcm::TetrMeshFirstOrder::build_volume_reverse_lookups() {
	LOG_DEBUG("Building volume reverse lookups");

	ElasticNode* node;
	TetrFirstOrder* tetr;
	// Init vectors for "reverse lookups" of tetrahedrons current node is a member of.
	//for(int i = 0; i < nodesNumber; i++) { 
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		node = getNode(i);
		node->elements->clear();
		//delete node->elements;
		//node->elements = new vector<int>; 
	}

	// Go through all the tetrahedrons
	//for(int i = 0; i < tetrsNumber; i++) {
	for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr ) {
		int i = itr->first;
		tetr = getTetr(i);
		// For all verticles
		for(int j = 0; j < 4; j++)
		{
			node = getNode( tetr->verts[j] );
			assert( node->isFirstOrder() );
			// Push to data of nodes the number of this tetrahedron
			node->elements->push_back( tetr->number );
		}
	}
}

void gcm::TetrMeshFirstOrder::check_numbering() {
	LOG_DEBUG("Checking numbering");

	// Check if internal numbers of nodes are the same as numbers in array
	// We need it in future to perform quick access to nodes in array
	//for(int i = 0; i < nodesNumber; i++)
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		if(getNode(i)->number != i)
			THROW_BAD_MESH("Invalid node numbering");
	}
	//for(int i = 0; i < tetrsNumber; i++) {
	for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr ) {
		int i = itr->first;
		if(getTetr(i)->number != i)
			THROW_BAD_MESH("Invalid tetrahedron numbering");
	}
	for(int i = 0; i < faceNumber; i++)
	{
		if(getTriangle(i)->number != i)
			THROW_BAD_MESH("Invalid triangle numbering");
	}
}

void gcm::TetrMeshFirstOrder::build_border() {
	// Prepare border data

	float solid_angle;
	float solid_angle_part;

	LOG_DEBUG("Looking for border nodes using angles");
	int nodeCount = 0;
	
	ElasticNode* node;
	TetrFirstOrder* tetr;
	
	// Check border using solid angle comparation with 4*PI

	//for( int i = 0; i < nodesNumber; i++ ) {
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		node = getNode(i);
		node->setIsBorder(false);
		// We consider remote nodes to be 'border' of current local ones
		if( /*node->isLocal() &&*/ node->isFirstOrder() )
		{
			solid_angle = 0;
			for(unsigned j = 0; j < node->elements->size(); j++)
			{
				solid_angle_part = get_solid_angle(i, node->elements->at(j));
				assert(solid_angle_part >= 0);
				solid_angle += solid_angle_part;
			}
			if( fabs(4 * M_PI - solid_angle) > M_PI * EQUALITY_TOLERANCE ) {
				node->setIsBorder (true);
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
		tetr = getTetr(i);
		for( int j = 0; j < 4; j++ )
		{
			if( isTriangleBorder( tetr->verts ) )
				faceCount++;
			shiftArrayLeft( tetr->verts, 4 );
		}
	}
	
	LOG_DEBUG("Found " << faceCount << " border triangles");
	
	createTriangles(faceCount);
	
	//for(int i = 0; i < tetrsNumber; i++) {
	for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr ) {
		int i = itr->first;
		tetr = getTetr(i);
		for( int j = 0; j < 4; j++ )
		{
			if( isTriangleBorder( tetr->verts ) )
			{
				*getTriangle(number) = createBorderTriangle( tetr->verts, number );
				number++;
			}
			shiftArrayLeft( tetr->verts, 4 );
		}
	}
	
	assert( number == faceCount );
	LOG_DEBUG("Created " << faceNumber << " triangles");
}

void gcm::TetrMeshFirstOrder::build_surface_reverse_lookups() {
	LOG_DEBUG("Building surface reverse lookups");

	ElasticNode* node;
	TriangleFirstOrder* tri;
	// Init vectors for "reverse lookups" of border triangles current node is a member of.
	//for(int i = 0; i < nodesNumber; i++) {
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		node = getNode(i);
		node->border_elements->clear();
		//delete node->border_elements;
		//node->border_elements = new vector<int>; 
	}

	// Go through all the triangles and push to data of nodes the number of this triangle
	for(int i = 0; i < faceNumber; i++)
	{
		tri = getTriangle(i);
		for(int j = 0; j < 3; j++)
			getNode( tri->verts[j] )->border_elements->push_back(i);
	}
}

void gcm::TetrMeshFirstOrder::check_unused_nodes() {
	LOG_DEBUG("Looking for unused nodes");

	ElasticNode* node;
	// Check all the nodes and find 'unused'

	// Case 1 - node has no connections at all
	//for(int i = 0; i < nodesNumber; i++) {
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		node = getNode(i);
		if( (node->elements)->size() == 0 )
			node->setUsed(false);
	}

	// TetrFirstOrder* tetr;
	// Case 2 - remote ones that have connections only with remote ones
	//for(int i = 0; i < nodesNumber; i++) {
	/*for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		node = getNode(i);
		// If node is remote
		if( node->isRemote() )
		{
			int count = 0;
			// Check tetrahedrons it is a member of
			for(unsigned j = 0; j < node->elements->size(); j++)
			{
				tetr = getTetr( node->elements->at(j) );
				// Check verticles
				for(int k = 0; k < 4; k++)
				{
					// If it is local - count++
					if( getNode( tetr->verts[k] )->isLocal() )
						count++;
				}
			}
			// If remote node is NOT connected with at least one local - it is unused one
			if(count == 0)
			{
				node->setUsed(false);
			}
		}
	}*/
}

void gcm::TetrMeshFirstOrder::check_outer_normals() {
	LOG_DEBUG("Checking nodes outer normals");

	// Normal vector
	float normal[3];
	// Displacement
	//float dx[3];
	
	ElasticNode* node;
	
	// Guaranteed allowed step
	//float step_h = get_min_h() * 0.5;

	// Check all nodes
	//for(int i = 0; i < nodesNumber; i++) {
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		node = getNode(i);
		if(node->isBorder() && node->isUsed()) {

			find_border_node_normal(i, &normal[0], &normal[1], &normal[2], false);

			// Displacement along normal
			//dx[0] = step_h * normal[0];
			//dx[1] = step_h * normal[1];
			//dx[2] = step_h * normal[2];

			// Check if we are inside of the body moving towards normal
			//if( find_owner_tetr(&nodes[i], -dx[0], -dx[1], -dx[2]) == NULL ) {
				// Smth bad happens
//				*logger << "Can not find outer normal\n";
//				*logger << nodes[i].coords[0] << " " << nodes[i].coords[1] << " " < nodes[i].coords[2];
//				*logger << normal[0] << " " << normal[1] << " " < normal[2];
//				*logger << dx[0] << " " << dx[1] << " " < dx[2];
//				throw GCMException( GCMException::MESH_EXCEPTION, "Can not find outer normal");
			//} else {
				// Outside body - normal is outer - do nothing
			//}
		}

	}
	LOG_DEBUG("Checking nodes outer normals done");
}

void gcm::TetrMeshFirstOrder::create_outline() {
	if (nodesNumber)
	{
		LOG_DEBUG("Creating outline");

		ElasticNode* node;
		
		// Create outline
		for(int j = 0; j < 3; j++)
		{
			outline.min_coords[j] = numeric_limits<float>::infinity();
			outline.max_coords[j] = - numeric_limits<float>::infinity();
			expandedOutline.min_coords[j] = numeric_limits<float>::infinity();
			expandedOutline.max_coords[j] = - numeric_limits<float>::infinity();
		}

		//for(int i = 1; i < nodesNumber; i++) {
		for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
			int i = itr->first;
			node = getNode(i);
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

void gcm::TetrMeshFirstOrder::verifyTetrahedraVertices ()
{
	TetrFirstOrder* tetr;
	LOG_DEBUG ("Verifying tetrahedra vertices");
	for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr ) {
		int tetrInd = itr->first;
		tetr = getTetr(tetrInd);
		for (int vertInd = 0; vertInd < 4; vertInd++)
		{
			assert( getNode( tetr->verts[vertInd] )->isFirstOrder() );
		}
	}
}

TriangleFirstOrder gcm::TetrMeshFirstOrder::createBorderTriangle(int v[4], int number)
{
	bool needSwap;
	bool isBorder;
	isBorder = isTriangleBorder( v, &needSwap );
	assert( isBorder );
	
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

bool gcm::TetrMeshFirstOrder::isTriangleBorder(int v[4])
{
	bool needSwap;
	return isTriangleBorder( v, &needSwap, false );
}

bool gcm::TetrMeshFirstOrder::isTriangleBorder(int v[4], bool debug)
{
	bool needSwap;
	return isTriangleBorder( v, &needSwap, debug );
}

bool gcm::TetrMeshFirstOrder::isTriangleBorder(int v[4], bool* needSwap)
{
	return isTriangleBorder( v, needSwap, false );
}

bool gcm::TetrMeshFirstOrder::isTriangleBorder(int v[4], bool* needSwap, bool debug)
{
	*needSwap = false;
	ElasticNode* v1 = getNode( v[0] );
	ElasticNode* v2 = getNode( v[1] );
	ElasticNode* v3 = getNode( v[2] );
	ElasticNode* tetr_vert = getNode( v[3] );

	if( !v1->isBorder() || !v2->isBorder() || !v3->isBorder() )
		return false;
	
	bool _debug = debug;
	//if( ( v1 == 0 && v2 == 421 ) || ( v1 == 0 && v3 == 421 ) || ( v2 == 0 && v3 == 421 ) 
	//		|| ( v2 == 0 && v1 == 421 ) || ( v3 == 0 && v1 == 421 ) || ( v3 == 0 && v2 == 421 ) )
	//	_debug = true;
	//if( v1 == 99 && v2 == 119 && v3 == 124 )
	//	debug = true;
	
	float h = get_min_h() * 0.25;
	
	// Normal vector
	float normal[3];
	// Displacement
	float dx[3];

	float center[3];
	float innerDirection[3];
	float plane_move[3];
	// float plane_move_mod;

	findTriangleFaceNormal(v1->coords, v2->coords, v3->coords, 
							&normal[0], &normal[1], &normal[2]);

	// Triangle center
	for(int i = 0; i < 3; i++)
		center[i] = (v1->coords[i] + v2->coords[i] + v3->coords[i]) / 3;

	// Direction from triangle center to the verticle of tetrahedron
	for(int i = 0; i < 3; i++)
		innerDirection[i] = tetr_vert->coords[i] - center[i];
	
	if( _debug )
	{
		LOG_DEBUG("Debug issue with isTriangleBorder()");
		LOG_DEBUG("Node #1: " << *v1);
		LOG_DEBUG("Node #2: " << *v2);
		LOG_DEBUG("Node #3: " << *v3);
		LOG_DEBUG("Add node: " << *tetr_vert);
		LOG_DEBUG("Normal: " << normal[0] << " " << normal[1] << " " << normal[2]);
		LOG_DEBUG("Center: " << center[0] << " " << center[1] << " " << center[2]);
		LOG_DEBUG("Inner direction: " << innerDirection[0] << " " << innerDirection[1] << " " << innerDirection[2]);
		LOG_DEBUG("Scalar product: " << scalarProduct( normal, innerDirection));
		LOG_DEBUG("MinH: " << get_min_h());
	}
	
	// Check if normal is co-linear with tetr verticle direction
	if( scalarProduct( normal, innerDirection) > 0 )
	{
		// If they are - invert normal because only opposite direction can be outer
		for(int i = 0; i < 3; i++)
			normal[i] = -normal[i];
		// And swap verticles order to match new direction
		v2 = getNode( v[2] );
		v3 = getNode( v[1] );
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
		plane_move[i] = center[i] - v1->coords[i];

	for(int i = 0; i < 3; i++)
		dx[i] += plane_move[i];

	if( _debug )
	{
		LOG_DEBUG("Base node: " << *v1);
		LOG_DEBUG("Move: " << dx[0] << " " << dx[1] << " " << dx[2]);
	}
	
	// Check if we are outside of the body moving from triangle center along normal ...
	if( /*find_owner_tetr*/fastScanForOwnerTetr(v1, dx[0], dx[1], dx[2], _debug) != -1 )
	{
		if(_debug)
			LOG_DEBUG("Result: not border");
		return false;
	}
	
	if(_debug)
		LOG_DEBUG("Result: border");
	
	return true;
};

/*TetrFirstOrder*/ int gcm::TetrMeshFirstOrder::findTargetPoint(ElasticNode* node, float dx, float dy, float dz, bool debug, float* coords, bool* innerPoint)
{
	//TetrFirstOrder* tetr;
	int tetr;
	if( vectorSquareNorm(dx, dy, dz) < mesh_min_h * mesh_min_h * (1 + EQUALITY_TOLERANCE) )
	{
		tetr = fastScanForOwnerTetr (node, dx, dy, dz, debug);
		coords[0] = node->coords[0] + dx;
		coords[1] = node->coords[1] + dy;
		coords[2] = node->coords[2] + dz;
		*innerPoint = (tetr != -1);
		return tetr;
	}
	else
	{
		return expandingScanForPoint (node, dx, dy, dz, debug, coords, innerPoint);
	}
}

/*TetrFirstOrder*/ int gcm::TetrMeshFirstOrder::find_owner_tetr(ElasticNode* node, float dx, float dy, float dz, bool debug)
{
	bool innerPoint;
	//TetrFirstOrder* tetr;
	int tetr;
	float coords[3];
	if( vectorSquareNorm(dx, dy, dz) < mesh_min_h * mesh_min_h * (1 + EQUALITY_TOLERANCE) )
		return fastScanForOwnerTetr (node, dx, dy, dz, debug);
	else
	{
		tetr = expandingScanForPoint (node, dx, dy, dz, debug, coords, &innerPoint);
		return ( innerPoint ? tetr : /*NULL*/-1 );
	}
}

/*TetrFirstOrder*/ int gcm::TetrMeshFirstOrder::fastScanForOwnerTetr(ElasticNode* node, float dx, float dy, float dz, bool debug)
{
	if( debug )
		LOG_TRACE("Fast scan - debug ON (however, consider !inAABB condition)");
	//assert( vectorSquareNorm(dx, dy, dz) <= mesh_min_h * mesh_min_h );
	
	TetrFirstOrder* tetr;
	float x = node->coords[0] + dx;
	float y = node->coords[1] + dy;
	float z = node->coords[2] + dz;
	
	// Disable it since it causes issues with parallel computation
	// - it becomes not clear how we should use here outline, expandedOutline, 'zone of interest'
	/*if( ! outline.isInAABB(x, y, z) )
	{
		if( debug )
			LOG_TRACE("Not in AABB");
		return -1;
	}*/
	
	for(unsigned i = 0; i < (node->elements)->size(); i++)
	{
		tetr = getTetr((node->elements)->at(i));
		if( pointInTetr(x, y, z, 
				getNode( tetr->verts[0] )->coords, getNode( tetr->verts[1] )->coords,
				getNode( tetr->verts[2] )->coords, getNode( tetr->verts[3] )->coords, 
				debug) )
		{
			//return tetr;
			return tetr->number;
		}
	}

	//return NULL;
	return -1;
}

/*TetrFirstOrder*/ int gcm::TetrMeshFirstOrder::expandingScanForPoint (ElasticNode* node, float dx, float dy, float dz, bool debug, float* coords, bool* innerPoint)
{
	if( debug )
		LOG_TRACE("Expanding scan - debug ON");
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
	
	float x = node->coords[0] + dx;
	float y = node->coords[1] + dy;
	float z = node->coords[2] + dz;
	
	float dist = sqrt(R2);
	float direction[3];
	direction[0] = dx / dist;
	direction[1] = dy / dist;
	direction[2] = dz / dist;	
	
	// if( ! outline.isInAABB(x, y, z) )
	//	return NULL;

	vector<int> tetrsToCheck;
	map<int, bool> checkedWithNeigh;
	map<int, bool>::const_iterator itr;
	
	// If current tetrs are inside sphere of radius R or not. If not - we should stop search and return NULL
	bool inside_R = true;
	bool last_inside_R = true;
	
	bool triangleIntersectionFound;
	
	float local_x, local_y, local_z;
	TetrFirstOrder* curTetr;

	for(unsigned i = 0; i < (node->elements)->size(); i++)
		tetrsToCheck.push_back( (node->elements)->at(i) );

	while(inside_R)
	{
		if( debug )
			LOG_TRACE("Starting next stage of scan");
		
		inside_R = last_inside_R;
		last_inside_R = false;
		triangleIntersectionFound = false;

		// Check current tetrs
		for(unsigned i = 0; i < tetrsToCheck.size(); i++)
		{
			curTetr = getTetr(tetrsToCheck[i]);
			assert( curTetr->number == tetrsToCheck[i] );

			// Check inside points of current tetr
			if( pointInTetr(x, y, z, 
					getNode( curTetr->verts[0] )->coords, 
					getNode( curTetr->verts[1] )->coords, 
					getNode( curTetr->verts[2] )->coords, 
					getNode( curTetr->verts[3] )->coords, 
					debug) )
			{
				// Tell that point found is inner and set coords
				coords[0] = x;	coords[1] = y;	coords[2] = z;
				*innerPoint = true;
				if( debug )
				{
					LOG_TRACE("Point is inner for tetr " << curTetr->number);
					LOG_TRACE("Expanding scan done");
				}
				//return curTetr;
				return curTetr->number;
			}
			
			// Check faces of current tetr
			for(int j = 0; j < 4; j++)
			{
				int i1 = (0+j) % 4;
				int i2 = (1+j) % 4;
				int i3 = (2+j) % 4;
				/*if( curTetr->verts[i1] == node->number 
						|| curTetr->verts[i2] == node->number 
						|| curTetr->verts[i3] == node->number )
					continue;*/
				
				ElasticNode* n1 = getNode( curTetr->verts[i1] );
				ElasticNode* n2 = getNode( curTetr->verts[i2] );
				ElasticNode* n3 = getNode( curTetr->verts[i3] );

				if( pointInTriangle( node->coords[0], node->coords[1], node->coords[2],
										n1->coords, n2->coords, n3->coords, false ) 
				)
					continue;
				
				bool intersectionCheckRequired = (
					( n1->isBorder() && n2->isBorder() && n3->isBorder() )
					|| !triangleIntersectionFound
				);
				
				if( intersectionCheckRequired 
						&& vectorIntersectsTriangle( n1->coords, n2->coords, n3->coords, 
													node->coords, direction, dist, coords, debug) 
				)
				{
					if ( n1->isBorder() && n2->isBorder() && n3->isBorder() )
					{
						// Tell that point found is border, 
						// coords are already set by vectorIntersectsTriangle()
						*innerPoint = false;
						if( debug )
						{
							LOG_TRACE("Intersection node: " << coords[0] << " " << coords[1] << " " << coords[2] );
							LOG_TRACE("Expanding scan done");
						}
						//return curTetr;
						return curTetr->number;
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
				// For all verticles of current tetr
				for(int j = 0; j < 4; j++)
				{
					ElasticNode* vertJ = getNode( curTetr->verts[j] );
					// Skip base node. Otherwise we'll get false positive insideR for the 1st and 2nd layers
					if( vertJ->number == node->number )
						continue;

					local_x = vertJ->coords[0];
					local_y = vertJ->coords[1];
					local_z = vertJ->coords[2];
					// If its distance smaller than R
					if( vectorSquareNorm( node->coords[0] - local_x,
							node->coords[1] - local_y, node->coords[2] - local_z) < R2 )
					{
							last_inside_R = true;
					}
					// FIXME In theory we whould check if sphere and tetr intersect.
					// Current check - if at least one vert is in sphere.
					// It can be wrong on turbo bad tetrs. It fails if
					// sphere goes 'through' tetr to next layer but does not include its verticles.
				}
			}
		}

		// If current layer is not in sphere - there is no need to create next layer - just return NULL
		if( !inside_R || ( node->isBorder() && !triangleIntersectionFound ) )
		{
			*innerPoint = false;
			if( debug )
			{
				LOG_TRACE("Point not found");
				LOG_TRACE("Expanding scan done");
			}
			//return NULL;
			return -1;
		}

		// If not found in current tetrs - create new list for checking
		int checkedTetrNum;
		bool isCompletelyChecked;
		TetrFirstOrder* checkedTetr;
		int nextTetrNum;

		tetrsToCheck.clear();
		
		for( itr = checkedWithNeigh.begin(); itr != checkedWithNeigh.end(); ++itr )
		{
			checkedTetrNum = itr->first;
			isCompletelyChecked = itr->second;
			if( isCompletelyChecked )
				continue;
			
			checkedTetr = getTetr(checkedTetrNum);
			for(int j = 0; j < 4; j++)
			{
				ElasticNode* vertJ = getNode( checkedTetr->verts[j] );
				for(unsigned k = 0; k < vertJ->elements->size(); k++)
				{
					nextTetrNum = vertJ->elements->at(k);
					if( checkedWithNeigh.find(nextTetrNum) == checkedWithNeigh.end() )
						tetrsToCheck.push_back(nextTetrNum);
				}
			}
			checkedWithNeigh[checkedTetrNum] = true;
		}
	}

	*innerPoint = false;
	if( debug )
	{
		LOG_TRACE("Point not found");
		LOG_TRACE("Expanding scan done");
	}
	//return NULL;
	return -1;
}

void gcm::TetrMeshFirstOrder::find_border_node_normal(int border_node_index, float* x, float* y, float* z, bool debug)
{
	ElasticNode* node = getNode( border_node_index );
	assert( node->isBorder() );
	
	float final_normal[3];
	final_normal[0] = 0;
	final_normal[1] = 0;
	final_normal[2] = 0;

	float cur_normal[3];

	int count = node->border_elements->size();
	int triNumber;
	
	float h = mesh_min_h * 0.5;
	
	if( count <= 0 )
	{
		LOG_WARN("Border node does not belong to any face");
		LOG_WARN("Can not find normal for the node " << *node);
	}

	for(int i = 0; i < count; i++)
	{
		triNumber = (int) node->border_elements->at(i);
		find_border_elem_normal( triNumber, &cur_normal[0], &cur_normal[1], &cur_normal[2] );
		
		final_normal[0] += cur_normal[0];
		final_normal[1] += cur_normal[1];
		final_normal[2] += cur_normal[2];
		if( debug )
		{
			LOG_TRACE("Element " << triNumber);
			LOG_TRACE("Vert #1: " << *(Node*)getNode( getTriangle(triNumber)->verts[0] ));
			LOG_TRACE("Vert #2: " << *(Node*)getNode( getTriangle(triNumber)->verts[1] ));
			LOG_TRACE("Vert #3: " << *(Node*)getNode( getTriangle(triNumber)->verts[2] ));
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
	
	if( find_owner_tetr(node, 
			- h * final_normal[0], - h * final_normal[1], - h * final_normal[2], debug) == -1 )
	{
		if( !debug )
		{
			LOG_TRACE("Sharp border - can not create normal for node: " << *node );
			LOG_TRACE("Normal: " << final_normal[0] << " " << final_normal[1] << " " << final_normal[2]);
			LOG_TRACE("Re-running search with debug on");
			//find_owner_tetr(node, - h * final_normal[0], - h * final_normal[1], - h * final_normal[2], true);
			find_border_node_normal(border_node_index, x, y, z, true);
		}
	}
}

float gcm::TetrMeshFirstOrder::get_solid_angle(int node_index, int tetr_index)
{
	TetrFirstOrder* tetr = getTetr(tetr_index);
	ElasticNode* node = getNode(node_index);
	// Node should belong to Tetr
	assert( ( tetr->verts[0] == node_index ) 
			|| ( tetr->verts[1] == node_index ) 
			|| ( tetr->verts[2] == node_index ) 
			|| ( tetr->verts[3] == node_index ) );
	/*{
		LOG_WARN("TEST: " << node_index << " " << tetr->verts[0] << " " << tetr->verts[1] << " " << tetr->verts[2] << " " << tetr->verts[3]);
		LOG_WARN("TEST: " << *node);
		LOG_WARN("TEST: " << *getNode( tetr->verts[0] ));
		LOG_WARN("TEST: " << *getNode( tetr->verts[1] ));
		LOG_WARN("TEST: " << *getNode( tetr->verts[2] ));
		LOG_WARN("TEST: " << *getNode( tetr->verts[3] ));
		return -1;
	}*/
	
	int verts[3];
	int count = 0;
	
	for(int i = 0; i < 4; i++)
		if(tetr->verts[i] != node_index) {
			verts[count] = tetr->verts[i];
			count++;
		}
	
	// We are to find 3 other verticles to form the face we are looking at
	assert( count == 3 );
	
	ElasticNode* v0 = getNode( verts[0] );
	ElasticNode* v1 = getNode( verts[1] );
	ElasticNode* v2 = getNode( verts[2] );
	
	return solidAngle(
			v0->coords[0] - node->coords[0],
			v0->coords[1] - node->coords[1],
			v0->coords[2] - node->coords[2],
			v1->coords[0] - node->coords[0], 
			v1->coords[1] - node->coords[1],
			v1->coords[2] - node->coords[2],
			v2->coords[0] - node->coords[0], 
			v2->coords[1] - node->coords[1],
			v2->coords[2] - node->coords[2]
	);
};

void gcm::TetrMeshFirstOrder::find_border_elem_normal(int border_element_index, 
														float* x, float* y, float* z)
{
	TriangleFirstOrder* tri = getTriangle(border_element_index);
	findTriangleFaceNormal( getNode( tri->verts[0] )->coords, 
							getNode( tri->verts[1] )->coords, 
							getNode( tri->verts[2] )->coords, 
							x, y, z );
};

void gcm::TetrMeshFirstOrder::calc_min_h()
{
	//assert( tetrsNumber > 0 );
	if( tetrsNumber == 0 )
		return;
	float min_h = tetr_h(tetrsMap.begin()->first);
	assert( min_h > 0 );
	
	float h;
	// Go through tetrahedrons
	//for(int i = 0; i < tetrsNumber; i++) {
	for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr ) {
		int i = itr->first;
		TetrFirstOrder* tetr;
		tetr = getTetr(i);
		if ( ( !getNode( tetr->verts[0] )->isUsed() )
			|| ( !getNode( tetr->verts[1] )->isUsed() )
			|| ( !getNode( tetr->verts[2] )->isUsed() )
			|| ( !getNode( tetr->verts[3] )->isUsed() ) )
			continue;
		
		// Get current h
		h = tetr_h(i);
		assert( h > 0 );
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

void gcm::TetrMeshFirstOrder::calc_max_h()
{
	//assert( tetrsNumber > 0 );
	if( tetrsNumber == 0 )
		return;
	float max_h = tetr_h(tetrsMap.begin()->first);
	assert( max_h > 0 );
	
	float h;
	// Go through tetrahedrons
	//for(int i = 0; i < tetrsNumber; i++) {
	for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr ) {
		int i = itr->first;
		TetrFirstOrder* tetr;
		tetr = getTetr(i);
		if ( ( !getNode( tetr->verts[0] )->isUsed() )
			|| ( !getNode( tetr->verts[1] )->isUsed() )
			|| ( !getNode( tetr->verts[2] )->isUsed() )
			|| ( !getNode( tetr->verts[3] )->isUsed() ) )
			continue;
		
		// Get current h
		h = tetr_h(i);
		assert( h > 0 );
		// Otherwise - just find minimum
		if(h > max_h) { max_h = h; }
	}
	mesh_max_h = max_h;
};

void gcm::TetrMeshFirstOrder::calc_avg_h()
{
	//assert( tetrsNumber > 0 );
	if( tetrsNumber == 0 )
		return;
	float avg_h = 0;
	
	float h;
	// Go through tetrahedrons
	//for(int i = 0; i < tetrsNumber; i++) {
	for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr ) {
		int i = itr->first;
		TetrFirstOrder* tetr;
		tetr = getTetr(i);
		if ( ( !getNode( tetr->verts[0] )->isUsed() )
			|| ( !getNode( tetr->verts[1] )->isUsed() )
			|| ( !getNode( tetr->verts[2] )->isUsed() )
			|| ( !getNode( tetr->verts[3] )->isUsed() ) )
			continue;
		
		// Get current h
		h = tetr_h(i);
		assert( h > 0 );
		// Otherwise - just find minimum
		avg_h += h;
	}
	mesh_avg_h = avg_h / tetrsNumber;
};

float gcm::TetrMeshFirstOrder::tetr_h(int i)
{
	TetrFirstOrder* tetr = getTetr(i);
	if( tetr == NULL )
		LOG_WARN("AAA: " << body->getEngine()->getRank() << " " << i);
	assert( tetr != NULL );
	assert( getNode(tetr->verts[0]) != NULL );
	assert( getNode(tetr->verts[1]) != NULL );
	assert( getNode(tetr->verts[2]) != NULL );
	assert( getNode(tetr->verts[3]) != NULL );
	return tetrHeight( getNode(tetr->verts[0])->coords, getNode(tetr->verts[1])->coords,
						getNode(tetr->verts[2])->coords, getNode(tetr->verts[3])->coords );
};

void gcm::TetrMeshFirstOrder::clearErrorFlags()
{
	ElasticNode* node;
	//for( int i = 0; i < nodesNumber; i++ ) {
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		node = getNode(i);
		if( node->isLocal() )
			node->clearErrorFlags();
	}
};

// TODO
void gcm::TetrMeshFirstOrder::do_next_part_step(float tau, int stage)
{
	LOG_DEBUG("Nodes: " << nodesNumber);
	LOG_DEBUG("Tetrs: " << tetrsNumber);
	LOG_DEBUG("Border: " << faceNumber);
	
	if( stage == 0 )
	{
		LOG_DEBUG("Clear error flags on all nodes");
		clearErrorFlags();
	}
	
	ElasticNode* node;
	// FIXME
	NumericalMethod *method = body->getEngine()->getNumericalMethod("InterpolationFixedAxis");
	method->setSpaceOrder(numericalMethodOrder);

	if( ! syncedArea.includes( &areaOfInterest ) )
	{
		LOG_ERROR("Area of interest: " << areaOfInterest);
		LOG_ERROR("Synced area: " << syncedArea);
		assert( syncedArea.includes( &areaOfInterest ) );
	}
	
	// Border nodes
	LOG_DEBUG("Processing border nodes");
	
	//for( int i = 0; i < nodesNumber; i++ ) {
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		node = getNode(i);
		if( node->isLocal() && node->isBorder() )
				method->doNextPartStep( node, getNewNode(i), tau, stage, this );
	}

	// Inner nodes
	LOG_DEBUG("Processing inner nodes");
	//for( int i = 0; i < nodesNumber; i++ ) {
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		node = getNode(i);
		if( node->isLocal() && node->isInner() )
				method->doNextPartStep( node, getNewNode(i), tau, stage, this );
	}
	
	LOG_DEBUG("Copying values");
	// Copy values
	//for( int i = 0; i < nodesNumber; i++ ) {
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		node = getNode(i);
		if( node->isLocal() )
			memcpy( node->values, getNewNode(i)->values, GCM_VALUES_SIZE * sizeof(float) );
	}
};

void gcm::TetrMeshFirstOrder::move_coords(float tau)
{
	LOG_DEBUG("Moving mesh coords");
	ElasticNode* node;
	ElasticNode* newNode;
	//for(int i = 0; i < nodesNumber; i++) {
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		node = getNode(i);
		if( node->isLocal() && node->isFirstOrder() )
		{
			newNode = getNewNode(i);
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
	calc_min_h();
};

// TODO
int gcm::TetrMeshFirstOrder::proceed_rheology()
{
	ElasticNode* node;
	RheologyCalculator* rheoCalc = body->getEngine()->getRheologyCalculator("DummyRheology");
	//for(int i = 0; i < nodesNumber; i++) {
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		node = getNode(i);
		if( node->isLocal() )
		{
			rheoCalc->doCalc(node, node);
		}
	}
	return 0;
};

/*TetrFirstOrder*/ int gcm::TetrMeshFirstOrder::find_border_cross(ElasticNode* node, float dx, float dy, float dz, bool debug, ElasticNode* cross)
{
	return find_border_cross(node, dx, dy, dz, debug, cross->coords);
};

/*TetrFirstOrder*/ int gcm::TetrMeshFirstOrder::find_border_cross(ElasticNode* node, float dx, float dy, float dz, bool debug, float* coords)
{
	bool innerPoint;
	/*TetrFirstOrder*/ int tetr = expandingScanForPoint (node, dx, dy, dz, debug, coords, &innerPoint);
	assert( !innerPoint );
	return tetr;
};

float gcm::TetrMeshFirstOrder::getMaxLambda()
{
	NumericalMethod *method = body->getEngine()->getNumericalMethod("InterpolationFixedAxis");
	float maxLambda = 0;
	//for( int i = 0; i < nodesNumber; i++ ) {
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		float lambda = method->getMaxLambda( getNode(i) );
		if( lambda > maxLambda )
			maxLambda = lambda;
	}
	return maxLambda;
}

float gcm::TetrMeshFirstOrder::getMaxPossibleTimeStep()
{
	NumericalMethod *method = body->getEngine()->getNumericalMethod("InterpolationFixedAxis");
	float maxLambda = 0;
	//for( int i = 0; i < nodesNumber; i++ ) {
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		float lambda = method->getMaxLambda( getNode(i) );
		if( lambda > maxLambda )
			maxLambda = lambda;
	}
	LOG_DEBUG( "Min H over mesh is " << get_min_h() );
	LOG_DEBUG( "Max lambda over mesh is " << maxLambda );
	LOG_DEBUG( "Courant time step is " << get_min_h() / maxLambda );
	return get_min_h() / maxLambda;
}

void gcm::TetrMeshFirstOrder::printBorder()
{
	LOG_DEBUG("Border size: " << faceNumber);
}

void gcm::TetrMeshFirstOrder::setInitialState(Area* area, float* values)
{
	ElasticNode* node;
	//for( int i = 0; i < nodesNumber; i++ ) {
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		node = getNode(i);
		if( area->isInArea( node ) )
			for( int k = 0; k < 9; k++ )
				node->values[k] = values[k];
	}
}

void gcm::TetrMeshFirstOrder::setRheology(float la, float mu, float rho) {
	ElasticNode* node;
	//for( int i = 0; i < nodesNumber; i++ ) {
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		node = getNode(i);
		node->la = la;
		node->mu = mu;
		node->rho = rho;
	}
}

void gcm::TetrMeshFirstOrder::setRheology(float la, float mu, float rho, AABB* box) {
	ElasticNode* node;
	//for( int i = 0; i < nodesNumber; i++ ) {
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		node = getNode(i);
		if( box->isInAABB(node) )
		{
			node->la = la;
			node->mu = mu;
			node->rho = rho;
		}
	}
}

void gcm::TetrMeshFirstOrder::logMeshStats()
{
	if( isinf( get_max_h() ) )
	{
		LOG_DEBUG("Mesh is empty");
		return;
	}
	/*float h = 0;
	
	float min_h = tetr_h( tetrsMap.begin()->first );
	float max_h = tetr_h( tetrsMap.begin()->first );
	float avg_h = 0;
	
	float hyst[10];
	hyst[0] = hyst[1] = hyst[2] = hyst[3] = hyst[4] = hyst[5] = hyst[6] = hyst[7] = hyst[8] = hyst[9] = 0;
	
	int num;*/
	
	LOG_INFO("Number of nodes: " << nodesNumber);
	LOG_INFO("Number of tetrs: " << tetrsNumber);

	/*for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr )
	{
		int i = itr->first;
		TetrFirstOrder* tetr;
		tetr = getTetr(i);
		if ( (!nodes[tetr->verts[0]].isUsed ())
			|| (!nodes[tetr->verts[1]].isUsed ())
			|| (!nodes[tetr->verts[2]].isUsed ())
			|| (!nodes[tetr->verts[3]].isUsed ()) )
			continue;
		
		// Get current h
		h = tetr_h(i);
		assert( h > 0 );
		if( h > max_h )
			max_h = h;
		if( h < min_h )
			min_h = h;
		avg_h += h;
	}
	avg_h /= tetrsNumber;
	
	for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr )
	{
		int i = itr->first;
		TetrFirstOrder* tetr;
		tetr = getTetr(i);
		if ( (!nodes[tetr->verts[0]].isUsed ())
			|| (!nodes[tetr->verts[1]].isUsed ())
			|| (!nodes[tetr->verts[2]].isUsed ())
			|| (!nodes[tetr->verts[3]].isUsed ()) )
			continue;
		
		// Get current h
		h = tetr_h(i);
		h = h / max_h;
		num = (int)(h/0.1);
		hyst[num]++;
	}*/

	LOG_INFO("Mesh outline:" << outline);
	LOG_INFO("Mesh expanded outline:" << expandedOutline);
	
	LOG_INFO("Mesh quality:");
	LOG_INFO("Max H = " << get_max_h());
	LOG_INFO("Min H = " << get_min_h());
	LOG_INFO("Avg H = " << get_avg_h());
	/*LOG_INFO("Histogramm:");
	for(int i = 0; i < 10; i++)
		LOG_INFO("\t" << max_h * i / 10 << " to " << max_h * (i+1) / 10 << ": " << hyst[i]);*/
};

void gcm::TetrMeshFirstOrder::checkTopology(float tau)
{
	LOG_DEBUG("Checking mesh topology");
	
	if( isinf( get_max_h() ) )
	{
		LOG_DEBUG("Mesh is empty");
		return;
	}
	
	IEngine* e = body->getEngine();
	GCMDispatcher* d = e->getDispatcher();
	int workers = e->getNumberOfWorkers();
	//int rank = e->getRank();
	
	// FIXME@avasyukov - rethink it	
	if( d->getOutline(0) == NULL )
	{
		LOG_WARN("FIXME: we need getOutline() back!");
		return;
	}

	float maxLambda = getMaxLambda();
	for( int i = 0; i < 3; i++ )
	{
		areaOfInterest.min_coords[i] = outline.min_coords[i] - 2 * maxLambda * tau;
		areaOfInterest.max_coords[i] = outline.max_coords[i] + 2 * maxLambda * tau;
	}

	// FIXME@avasyukov - rethink it	
	if( isinf( syncedArea.minX ) )
	{
		memcpy( syncedArea.min_coords, outline.min_coords, 3 * sizeof(float) );
		memcpy( syncedArea.max_coords, outline.max_coords, 3 * sizeof(float) );
	}
	
	/*
	memcpy( syncedArea.min_coords, expandedOutline.min_coords, 3 * sizeof(float) );
	memcpy( syncedArea.max_coords, expandedOutline.max_coords, 3 * sizeof(float) );
	
	ElasticNode* node;
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr )
	{
		int i = itr->first;
		node = getNode(i);
		if( !node->isBorder() )
			continue;
		
		for( int j = 0; j < 3; j++ )
		{
			int targetCoord = (0 + j) % 3;
			int coord1 = (1 + j) % 3;
			int coord2 = (2 + j) % 3;
			
			if( ( node->coords[coord1] < outline.max_coords[coord1] )
					&& ( node->coords[coord1] > outline.min_coords[coord1] )
					&& ( node->coords[coord2] < outline.max_coords[coord2] )
					&& ( node->coords[coord2] > outline.min_coords[coord2] )
					&& ( node->coords[targetCoord] > outline.max_coords[targetCoord] )
					&& ( node->coords[targetCoord] < syncedArea.max_coords[targetCoord] )
				)
			{
				LOG_DEBUG("First pass: synced area is adjusted by node: "<< *node);
				syncedArea.max_coords[targetCoord] = node->coords[targetCoord];
			}
			
			if( ( node->coords[coord1] < outline.max_coords[coord1] )
					&& ( node->coords[coord1] > outline.min_coords[coord1] )
					&& ( node->coords[coord2] < outline.max_coords[coord2] )
					&& ( node->coords[coord2] > outline.min_coords[coord2] )
					&& ( node->coords[targetCoord] < outline.min_coords[targetCoord] )
					&& ( node->coords[targetCoord] > syncedArea.min_coords[targetCoord] )
				)
			{
				LOG_DEBUG("First pass: synced area is adjusted by node: "<< *node);
				syncedArea.min_coords[targetCoord] = node->coords[targetCoord];
			}
		}
	}
	
	LOG_DEBUG("First pass done. Synced area: " << syncedArea);
	
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr )
	{
		int i = itr->first;
		node = getNode(i);
		if( !node->isBorder() )
			continue;
		
		if( syncedArea.isInAABB(node) )
		{
			int indMin = -1;
			int indMax = -1;
			float distMin = numeric_limits<float>::infinity();
			float distMax = numeric_limits<float>::infinity();
			for( int j = 0; j < 3; j++ )
			{
				if( fabs(node->coords[j] - syncedArea.min_coords[j]) < distMin )
				{
					distMin = fabs(node->coords[j] - syncedArea.min_coords[j]);
					indMin = j;
				}
				if( fabs(node->coords[j] - syncedArea.max_coords[j]) < distMax )
				{
					distMax = fabs(node->coords[j] - syncedArea.max_coords[j]);
					indMax = j;
				}
			}
			if( distMin < distMax )
				syncedArea.min_coords[indMin] = node->coords[indMin];
			else
				syncedArea.max_coords[indMax] = node->coords[indMax];
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
	
	LOG_DEBUG("Mesh outline: " << outline);
	LOG_DEBUG("Mesh expanded outline: " << expandedOutline);
	LOG_DEBUG("Mesh area of interest: " << areaOfInterest);
	LOG_DEBUG("Mesh synced area: " << syncedArea);
}

float gcm::TetrMeshFirstOrder::getRecommendedTimeStep() {
	return get_avg_h() / getMaxLambda();
};

void gcm::TetrMeshFirstOrder::interpolate(ElasticNode* node, TetrFirstOrder* tetr) {
	body->getEngine()->getFirstOrderInterpolator("TetrFirstOrderInterpolator")->
	interpolate(
				node,
				getNode( tetr->verts[0] ),
				getNode( tetr->verts[1] ),
				getNode( tetr->verts[2] ),
				getNode( tetr->verts[3] ) 
	);
}

float gcm::TetrMeshFirstOrder::get_min_h()
{
	if( isinf( mesh_min_h ) )
		calc_min_h();
	return mesh_min_h;
}

float gcm::TetrMeshFirstOrder::get_max_h()
{
	if( isinf( mesh_max_h ) )
		calc_max_h();
	return mesh_max_h;
}

float gcm::TetrMeshFirstOrder::get_avg_h()
{
	if( isinf( mesh_avg_h ) )
		calc_avg_h();
	return mesh_avg_h;
}

void gcm::TetrMeshFirstOrder::transfer(float x, float y, float z)
{
	ElasticNode* node;
	for( MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr ) {
		int i = itr->first;
		node = getNode(i);
		node->coords[0] += x;
		node->coords[1] += y;
		node->coords[2] += z;
	}
	if( !isinf(outline.minX) )
	{
		outline.min_coords[0] += x;
		outline.min_coords[1] += y;
		outline.min_coords[2] += z;
		outline.max_coords[0] += x;
		outline.max_coords[1] += y;
		outline.max_coords[2] += z;
	}
	if( !isinf(expandedOutline.minX) )
	{
		expandedOutline.min_coords[0] += x;
		expandedOutline.min_coords[1] += y;
		expandedOutline.min_coords[2] += z;
		expandedOutline.max_coords[0] += x;
		expandedOutline.max_coords[1] += y;
		expandedOutline.max_coords[2] += z;
	}
	if( !isinf(syncedArea.minX) )
	{
		syncedArea.min_coords[0] += x;
		syncedArea.min_coords[1] += y;
		syncedArea.min_coords[2] += z;
		syncedArea.max_coords[0] += x;
		syncedArea.max_coords[1] += y;
		syncedArea.max_coords[2] += z;
	}
	if( !isinf(areaOfInterest.minX) )
	{
		areaOfInterest.min_coords[0] += x;
		areaOfInterest.min_coords[1] += y;
		areaOfInterest.min_coords[2] += z;
		areaOfInterest.max_coords[0] += x;
		areaOfInterest.max_coords[1] += y;
		areaOfInterest.max_coords[2] += z;
	}
	
	// TODO@avasyukov - think about additional checks
	body->getEngine()->transferScene(x, y, z);
}