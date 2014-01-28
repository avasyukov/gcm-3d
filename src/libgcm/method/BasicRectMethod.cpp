#include "BasicRectMethod.h"
#include "../node/CalcNode.h"

gcm::BasicRectMethod::BasicRectMethod()
{
	INIT_LOGGER("gcm.method.BasicRectMethod");
}

gcm::BasicRectMethod::~BasicRectMethod()
{
}

string gcm::BasicRectMethod::getType()
{
	return "BasicRectMethod";
}

int gcm::BasicRectMethod::getNumberOfStages()
{
	return 3;
}

float gcm::BasicRectMethod::getMaxLambda(CalcNode* node)
{
	assert( node != NULL );
	assert ( node->rheologyIsValid() );
	return sqrt( ( (node->getLambda()) + 2 * (node->getMu()) ) / (node->getRho()) );
}

void gcm::BasicRectMethod::doNextPartStep(CalcNode* cur_node, CalcNode* new_node, float time_step, int stage, Mesh* genericMesh)
{
	// FIXME - WA
	BasicCubicMesh* mesh = (BasicCubicMesh*)genericMesh;
	
	assert( stage >= 0 && stage <= 2 );
	
	IEngine* engine = mesh->getBody()->getEngine();
	
	LOG_TRACE("Start node prepare for node " << cur_node->number);
	LOG_TRACE("Node: " << *cur_node);

	// Used for real node
	ElasticMatrix3D elastic_matrix3d;

	// Variables used in calculations internally

	// Delta x on previous time layer for all the omegas
	// 	omega_new_time_layer(ksi) = omega_old_time_layer(ksi+dksi)
	float dksi[9];

	// If the corresponding point on previous time layer is inner or not
	bool inner[9];

	// We will store interpolated nodes on previous time layer here
	// We know that we need five nodes for each direction (corresponding to Lambdas -C1, -C2, 0, C2, C1)
	// TODO  - We can  deal with (lambda == 0) separately
	CalcNode previous_nodes[5];

	// Outer normal at current point
	float outer_normal[3];

	// This array will link omegas with corresponding interpolated nodes they should be copied from
	int ppoint_num[9];
	
	// Number of outer characteristics
	int outer_count = prepare_node(cur_node, &elastic_matrix3d, 
						time_step, stage, mesh, 
						dksi, inner, previous_nodes,
						outer_normal, ppoint_num);

	LOG_TRACE("Done node prepare");
	
	float* previous_values[9];
	for(int i = 0; i < 9; i++)
	{
		previous_values[i] = previous_nodes[ppoint_num[i]].values;
		LOG_TRACE("Ind " << i << ": " << ppoint_num[i]);
	}

	// If all the omegas are 'inner'
	// omega = Matrix_OMEGA * u
	// new_u = Matrix_OMEGA^(-1) * omega
	// TODO - to think - if all omegas are 'inner' can we skip matrix calculations and just use new_u = interpolated_u ?
	if( cur_node->isInner() )
	{
		LOG_TRACE("Start inner node calc");
		if( outer_count == 0 )
			// FIXME - hardcoded name
			engine->getVolumeCalculator("SimpleVolumeCalculator")->do_calc(
									new_node, &elastic_matrix3d, previous_values);
		else
			THROW_BAD_MESH("Outer characteristic for internal node detected");
		LOG_TRACE("Done inner node calc");
	}
	
	if( cur_node->isBorder() )
	{
		LOG_TRACE("Start border node calc");
		// FIXME - do smth with this!
		// Think about: (a) cube, (b) rotated cube, (c) sphere.
		outer_normal[0] = outer_normal[1] = outer_normal[2] = 0;
		outer_normal[stage] = 1;
		// If there is no 'outer' omega - it is ok, border node can be inner for some directions
		if( outer_count == 0 )
		{
			// FIXME - hardcoded name
			engine->getVolumeCalculator("SimpleVolumeCalculator")->do_calc(
									new_node, &elastic_matrix3d, previous_values);
		}
		// If there are 3 'outer' omegas - we should use border or contact algorithm
		else if ( outer_count == 3 )
		{
			// Border
			if( ! cur_node->isInContact() || cur_node->contactDirection != stage )
			{
				// FIXME
				int borderCondId = cur_node->getBorderConditionId();
				LOG_TRACE("Using calculator: " << engine->getBorderCondition(borderCondId)->calc->getType());
				engine->getBorderCondition(borderCondId)->do_calc(mesh->get_current_time(), cur_node, 
					new_node, &elastic_matrix3d, previous_values, inner, outer_normal);
			}
			// Contact
			// TODO - not implemented
			
		// It means smth went wrong. Just interpolate the values and report bad node.
		} else {
			// FIXME - implement border and contact completely
			LOG_TRACE("Using calculator: " << engine->getBorderCondition(0)->calc->getType());
			engine->getBorderCondition(0)->do_calc(mesh->get_current_time(), cur_node, 
					new_node, &elastic_matrix3d, previous_values, inner, outer_normal);
			cur_node->setNeighError(stage);
		}
		LOG_TRACE("Done border node calc");
	}
}

int gcm::BasicRectMethod::prepare_node(CalcNode* cur_node, ElasticMatrix3D* elastic_matrix3d,
												float time_step, int stage, BasicCubicMesh* mesh, 
												float* dksi, bool* inner, CalcNode* previous_nodes, 
												float* outer_normal, int* ppoint_num)
{
	assert( stage >= 0 && stage <= 2 );
	
	if( cur_node->isBorder() )
		mesh->findBorderNodeNormal(cur_node->number, &outer_normal[0], &outer_normal[1], &outer_normal[2], false);

	LOG_TRACE("Preparing elastic matrix");
	//  Prepare matrixes  A, Lambda, Omega, Omega^(-1)
	elastic_matrix3d->prepare_matrix( cur_node->getLambda(), cur_node->getMu(), cur_node->getRho(), stage );
	LOG_TRACE("Preparing elastic matrix done");
	
	LOG_TRACE("Elastic matrix eigen values:\n" << elastic_matrix3d->L);

	for(int i = 0; i < 9; i++)
		dksi[i] = - elastic_matrix3d->L(i,i) * time_step;

	return find_nodes_on_previous_time_layer(cur_node, stage, mesh, dksi, inner, previous_nodes, outer_normal, ppoint_num);
};

int gcm::BasicRectMethod::find_nodes_on_previous_time_layer(CalcNode* cur_node, int stage, BasicCubicMesh* mesh, 
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
			CalcNode* origin = mesh->getNode(cur_node->number);
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
				int neighInd = mesh->findTargetPoint( origin, dx[0], dx[1], dx[2], false, 
									previous_nodes[count].coords, &isInnerPoint );
				if( !isInnerPoint )
				{
					LOG_TRACE("Inner node: we need new method here!");
					LOG_TRACE("Node:\n" << *cur_node);
					LOG_TRACE("Move: " << dx[0] << " " << dx[1] << " " << dx[2]);
					// Re-run search with debug on
					neighInd = mesh->findTargetPoint( origin, dx[0], dx[1], dx[2], true, 
									previous_nodes[count].coords, &isInnerPoint );
				}
				
				interpolateNode(mesh, cur_node->number, neighInd, count, previous_nodes);
				
				inner[i] = true;
				LOG_TRACE( "Checking inner node done" );
			}
			else if( cur_node->isBorder() )
			{
				LOG_TRACE( "Checking border node" );
				// ... Find owner tetrahedron ...
				bool isInnerPoint;
				int neighInd = mesh->findTargetPoint( origin, dx[0], dx[1], dx[2], true, 
									previous_nodes[count].coords, &isInnerPoint );
				
				//TetrFirstOrder* tmp_tetr = ( neighInd != -1 ? mesh->getTetr(neighInd) : NULL );
				
				// If we found inner point, it means
				// this direction is inner and everything works as for usual inner point
				if( isInnerPoint ) {
					interpolateNode(mesh, cur_node->number, neighInd, count, previous_nodes);
					inner[i] = true;
				// If we did not find inner point - two cases are possible
				} else {
					// We found border cross somehow
					// It can happen if we work with really thin structures and big time step
					// We can work as usual in this case
					if( neighInd != -1 ) {
						LOG_TRACE("Border node: we need new method here!");
						interpolateNode(mesh, cur_node->number, neighInd, count, previous_nodes);
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

void gcm::BasicRectMethod::interpolateNode(BasicCubicMesh* mesh, int curInd, int neighInd, int prevNodeInd, CalcNode* previous_nodes)
{
	assert( curInd >= 0 && neighInd >= 0 );
	IEngine* engine = mesh->getBody()->getEngine();
	
	engine->getFirstOrderLineInterpolator("LineFirstOrderInterpolator")->interpolate(
			&previous_nodes[prevNodeInd], 
			(CalcNode*) mesh->getNode( curInd ),
			(CalcNode*) mesh->getNode( neighInd ) 
	);
};