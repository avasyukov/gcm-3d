#include "mesh/cube/BasicCubicMesh.h"

#include "node/CalcNode.h"

gcm::BasicCubicMesh::BasicCubicMesh()
{
	meshH = numeric_limits<float>::infinity();
	// FIXME - hardcoded name
	numericalMethodType = "InterpolationFixedAxis";
	// FIXME - hardcoded parameter
	numericalMethodOrder = 1;
	snapshotWriterType = "VTKCubicSnapshotWriter";
	dumpWriterType = "VTKCubicSnapshotWriter";
	INIT_LOGGER("gcm.BasicCubicMesh");
	LOG_DEBUG("Creating mesh");
}

gcm::BasicCubicMesh::~BasicCubicMesh()
{
	LOG_DEBUG("Destroying mesh '" << getId() << "'");
	// TODO - does it really trigger destructors?
	// TODO - do we need it here?
	nodes.clear();
	new_nodes.clear();
	LOG_DEBUG("Mesh destroyed");
}

void gcm::BasicCubicMesh::preProcessGeometry()
{
	CalcNode* node;
	LOG_DEBUG("Preprocessing mesh geometry started.");
	for(int i = 0; i < getNodesNumber(); i++)
	{
		node = getNodeByLocalIndex(i);
		for( int i = 0; i < 3; i ++)
		{
			if( ( fabs(node->coords[i] - outline.min_coords[i]) < EQUALITY_TOLERANCE )
				|| ( fabs(node->coords[i] - outline.max_coords[i]) < EQUALITY_TOLERANCE ) )
			{
				node->setIsBorder(true);
			}
		}
	}
	LOG_DEBUG("Preprocessing mesh geometry done.");
}

void gcm::BasicCubicMesh::doNextPartStep(float tau, int stage)
{
	defaultNextPartStep(tau, stage);
};

void gcm::BasicCubicMesh::logMeshStats()
{
	if( isinf( getMinH() ) )
	{
		LOG_DEBUG("Mesh is empty");
		return;
	}
	
	LOG_DEBUG("Number of nodes: " << nodesNumber);
	LOG_DEBUG("Mesh h: " << getMinH());
	LOG_DEBUG("Mesh outline:" << outline);
	LOG_DEBUG("Mesh expanded outline:" << expandedOutline);
};

void gcm::BasicCubicMesh::checkTopology(float tau)
{
	// We do not need it for this mesh type
}

float gcm::BasicCubicMesh::getRecommendedTimeStep()
{
	return getMinH() / getMaxLambda();
};

void gcm::BasicCubicMesh::calcMinH()
{
	if( getNodesNumber() < 2)
		return;
	
	CalcNode* base = getNodeByLocalIndex(0);
	CalcNode* node;
	float h;
	
	// We suppose that mesh is uniform
	for(int i = 1; i < getNodesNumber(); i++)
	{
		node = getNodeByLocalIndex(i);
		h = distance(base->coords, node->coords);
		if( h < meshH )
			meshH = h;
	}
	
	// TODO - we should auto-scale mesh transparently in this case
	if( meshH < EQUALITY_TOLERANCE )
	{
		LOG_WARN("Mesh minH is too small: minH " << meshH << ", FP tolerance: " << EQUALITY_TOLERANCE);
		LOG_WARN("Fixing it automatically, but it can cause numerous intersting issues");
		meshH = 10 * EQUALITY_TOLERANCE;
	}
};

float gcm::BasicCubicMesh::getMinH()
{
	if( isinf( meshH ) )
		calcMinH();
	return meshH;
};

void gcm::BasicCubicMesh::findBorderNodeNormal(int border_node_index, float* x, float* y, float* z, bool debug)
{
	CalcNode* node = getNode( border_node_index );
	assert( node->isBorder() );
	float normal[3];
	normal[0] = normal[1] = normal[2] = 0.0;
	for( int i = 0; i < 3; i ++)
	{
		if( fabs(node->coords[i] - outline.min_coords[i]) < EQUALITY_TOLERANCE )
		{
			normal[i] = -1;
			break;
		}
		if( fabs(node->coords[i] - outline.max_coords[i]) < EQUALITY_TOLERANCE )
		{
			normal[i] = 1;
			break;
		}
	}
	*x = normal[0];
	*y = normal[1];
	*z = normal[2];
};

int gcm::BasicCubicMesh::findTargetPoint(CalcNode* node, float dx, float dy, float dz, bool debug, float* coords, bool* innerPoint)
{
	int meshSize = 1 + (outline.maxX - outline.minX + meshH * 0.1) / meshH;
	
	assert( vectorSquareNorm(dx, dy, dz) <= getMinH() * getMinH() * (1 + EQUALITY_TOLERANCE) );
	
	coords[0] = node->coords[0] + dx;
	coords[1] = node->coords[1] + dy;
	coords[2] = node->coords[2] + dz;
	
	if( !outline.isInAABB(coords[0], coords[1], coords[2]) )
	{
		//LOG_INFO("OUT");
		coords[0] = node->coords[0];
		coords[1] = node->coords[1];
		coords[2] = node->coords[2];
		*innerPoint = false;
		return -1;
	}
	
	int neighNum = node->number;
	if ( dx > EQUALITY_TOLERANCE )
		neighNum += meshSize*meshSize;
	else if ( dx < -EQUALITY_TOLERANCE )
		neighNum -= meshSize*meshSize;
	else if ( dy > EQUALITY_TOLERANCE )
		neighNum += meshSize;
	else if ( dy < -EQUALITY_TOLERANCE )
		neighNum -= meshSize;
	else if ( dz > EQUALITY_TOLERANCE )
		neighNum += 1;
	else if ( dz < -EQUALITY_TOLERANCE )
		neighNum -= 1;
	
	*innerPoint = true;
	return neighNum;
};

int gcm::BasicCubicMesh::prepare_node(CalcNode* cur_node, ElasticMatrix3D* elastic_matrix3d,
												float time_step, int stage,
												float* dksi, bool* inner, CalcNode* previous_nodes,
												float* outer_normal, int* ppoint_num)
{
	assert( stage >= 0 && stage <= 2 );

	if( cur_node->isBorder() )
		findBorderNodeNormal(cur_node->number, &outer_normal[0], &outer_normal[1], &outer_normal[2], false);

	LOG_TRACE("Preparing elastic matrix");
	//  Prepare matrixes  A, Lambda, Omega, Omega^(-1)
	elastic_matrix3d->prepare_matrix( cur_node->getLambda(), cur_node->getMu(), cur_node->getRho(), stage );
	LOG_TRACE("Preparing elastic matrix done");

	LOG_TRACE("Elastic matrix eigen values:\n" << elastic_matrix3d->L);

	for(int i = 0; i < 9; i++)
		dksi[i] = - elastic_matrix3d->L(i,i) * time_step;

	return find_nodes_on_previous_time_layer(cur_node, stage, dksi, inner, previous_nodes, outer_normal, ppoint_num);
};

int gcm::BasicCubicMesh::find_nodes_on_previous_time_layer(CalcNode* cur_node, int stage,
												float dksi[], bool inner[], CalcNode previous_nodes[],
												float outer_normal[], int ppoint_num[])
{
	LOG_TRACE("Start looking for nodes on previous time layer");

	int count = 0;

	// For all omegas
	for(int i = 0; i < 9; i++)
	{
		LOG_TRACE( "Looking for characteristic " << i << " Count = " << count );
		// Check prevoius omegas ...
		bool already_found = false;
		for(int j = 0; j < i; j++)
		{
			// ... And try to find if we have already worked with the required point
			// on previous time layer (or at least with the point that is close enough)
			if( fabs(dksi[i] - dksi[j]) <= EQUALITY_TOLERANCE * 0.5 * fabs(dksi[i] + dksi[j]) )
			{
				// If we have already worked with this point - just remember the number
				already_found = true;
				ppoint_num[i] = ppoint_num[j];
				inner[i] = inner[j];
				LOG_TRACE( "Found old value " << dksi[i] << " - done" );
			}
		}

		// If we do not have necessary point in place - ...
		if( !already_found )
		{
			LOG_TRACE( "New value " << dksi[i] << " - preparing vectors" );
			// ... Put new number ...
			ppoint_num[i] = count;
			previous_nodes[count] = *cur_node;

			// ... Find vectors ...
			float dx[3];
			// WA:
			//     origin == cur_node for real nodes
			//     origin != cure_node for virt nodes
			CalcNode* origin = getNode(cur_node->number);
			for( int z = 0; z < 3; z++ )
			{
				dx[z] = cur_node->coords[z] - origin->coords[z];
			}
			dx[stage] += dksi[i];

			// For dksi = 0 we can skip check and just copy everything
			if( dksi[i] == 0 )
			{
				// no interpolation required - everything is already in place
				inner[i] = true;
				LOG_TRACE( "dksi is zero - done" );
			}
			else if( cur_node->isInner() )
			{
				LOG_TRACE( "Checking inner node" );
				// ... Find owner tetrahedron ...
				bool isInnerPoint;
				int neighInd = findTargetPoint( origin, dx[0], dx[1], dx[2], false,
									previous_nodes[count].coords, &isInnerPoint );
				if( !isInnerPoint )
				{
					LOG_TRACE("Inner node: we need new method here!");
					LOG_TRACE("Node:\n" << *cur_node);
					LOG_TRACE("Move: " << dx[0] << " " << dx[1] << " " << dx[2]);
					// Re-run search with debug on
					neighInd = findTargetPoint( origin, dx[0], dx[1], dx[2], true,
									previous_nodes[count].coords, &isInnerPoint );
				}

				interpolateNode(cur_node->number, neighInd, count, previous_nodes);

				inner[i] = true;
				LOG_TRACE( "Checking inner node done" );
			}
			else if( cur_node->isBorder() )
			{
				LOG_TRACE( "Checking border node" );
				// ... Find owner tetrahedron ...
				bool isInnerPoint;
				int neighInd = findTargetPoint( origin, dx[0], dx[1], dx[2], true,
									previous_nodes[count].coords, &isInnerPoint );

				//TetrFirstOrder* tmp_tetr = ( neighInd != -1 ? mesh->getTetr(neighInd) : NULL );

				// If we found inner point, it means
				// this direction is inner and everything works as for usual inner point
				if( isInnerPoint ) {
					interpolateNode(cur_node->number, neighInd, count, previous_nodes);
					inner[i] = true;
				// If we did not find inner point - two cases are possible
				} else {
					// We found border cross somehow
					// It can happen if we work with really thin structures and big time step
					// We can work as usual in this case
					if( neighInd != -1 ) {
						LOG_TRACE("Border node: we need new method here!");
						interpolateNode(cur_node->number, neighInd, count, previous_nodes);
						inner[i] = true;
					// Or we did not find any point at all - it means this characteristic is outer
					} else {
						inner[i] = false;
					}
				}
				LOG_TRACE( "Checking border node done" );
			}
			else
			{
				THROW_BAD_MESH("Unsupported case for characteristic location");
			}

			count++;
		}
		LOG_TRACE( "Looking for characteristic " << i << " done" );
	}

	assert( count == 5 || count == 3 );

	int outer_count = 0;
	for(int i = 0; i < 9; i++)
		if(!inner[i])
			outer_count++;

	// assert( outer_count == 0 || outer_count == 3 );

	LOG_TRACE("Looking for nodes on previous time layer done. Outer count = " << outer_count);
	for(int i = 0; i < 9; i++)
		LOG_TRACE("Characteristic " << i << " goes into point with index " << ppoint_num[i]);

	return outer_count;
};

void gcm::BasicCubicMesh::interpolateNode(int curInd, int neighInd, int prevNodeInd, CalcNode* previous_nodes)
{
	assert( curInd >= 0 && neighInd >= 0 );
	IEngine* engine = getBody()->getEngine();

	engine->getFirstOrderLineInterpolator("LineFirstOrderInterpolator")->interpolate(
			&previous_nodes[prevNodeInd],
			(CalcNode*) getNode( curInd ),
			(CalcNode*) getNode( neighInd )
	);
};

