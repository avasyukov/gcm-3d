/* 
 * File:   TetrFirstOrderInterpolator.cpp
 * Author: anganar
 * 
 * Created on May 3, 2013, 12:13 AM
 */

#include "TetrFirstOrderInterpolator.h"

gcm::TetrFirstOrderInterpolator::TetrFirstOrderInterpolator() {
	type = "TetrFirstOrderInterpolator";
	INIT_LOGGER( "gcm.TetrFirstOrderInterpolator" );
}

gcm::TetrFirstOrderInterpolator::~TetrFirstOrderInterpolator() {
}

void gcm::TetrFirstOrderInterpolator::interpolate(ElasticNode* node, ElasticNode* node0, ElasticNode* node1, ElasticNode* node2, ElasticNode* node3)
{
	LOG_TRACE("Start interpolation");
	assert( node != NULL );
	assert( node0 != NULL && node1 != NULL && node2 != NULL && node3 !=NULL );
	
	float Vol = tetrVolume(
		(node1->coords[0])-(node0->coords[0]),
		(node1->coords[1])-(node0->coords[1]),
		(node1->coords[2])-(node0->coords[2]),
		(node2->coords[0])-(node0->coords[0]),
		(node2->coords[1])-(node0->coords[1]),
		(node2->coords[2])-(node0->coords[2]),
		(node3->coords[0])-(node0->coords[0]),
		(node3->coords[1])-(node0->coords[1]),
		(node3->coords[2])-(node0->coords[2])
	);

	float factor[4];

	factor[0] = fabs( tetrVolume(
		(node1->coords[0])-(node->coords[0]),
		(node1->coords[1])-(node->coords[1]),
		(node1->coords[2])-(node->coords[2]),
		(node2->coords[0])-(node->coords[0]),
		(node2->coords[1])-(node->coords[1]),
		(node2->coords[2])-(node->coords[2]),
		(node3->coords[0])-(node->coords[0]),
		(node3->coords[1])-(node->coords[1]),
		(node3->coords[2])-(node->coords[2])
	) / Vol);

	factor[1] = fabs( tetrVolume(
		(node0->coords[0])-(node->coords[0]),
		(node0->coords[1])-(node->coords[1]),
		(node0->coords[2])-(node->coords[2]),
		(node2->coords[0])-(node->coords[0]),
		(node2->coords[1])-(node->coords[1]),
		(node2->coords[2])-(node->coords[2]),
		(node3->coords[0])-(node->coords[0]),
		(node3->coords[1])-(node->coords[1]),
		(node3->coords[2])-(node->coords[2])
	) / Vol);

	factor[2] = fabs( tetrVolume(
		(node1->coords[0])-(node->coords[0]),
		(node1->coords[1])-(node->coords[1]),
		(node1->coords[2])-(node->coords[2]),
		(node0->coords[0])-(node->coords[0]),
		(node0->coords[1])-(node->coords[1]),
		(node0->coords[2])-(node->coords[2]),
		(node3->coords[0])-(node->coords[0]),
		(node3->coords[1])-(node->coords[1]),
		(node3->coords[2])-(node->coords[2])
	) / Vol);

	factor[3] = fabs( tetrVolume(
		(node1->coords[0])-(node->coords[0]),
		(node1->coords[1])-(node->coords[1]),
		(node1->coords[2])-(node->coords[2]),
		(node2->coords[0])-(node->coords[0]),
		(node2->coords[1])-(node->coords[1]),
		(node2->coords[2])-(node->coords[2]),
		(node0->coords[0])-(node->coords[0]),
		(node0->coords[1])-(node->coords[1]),
		(node0->coords[2])-(node->coords[2])
	) / Vol);

	// If we see potential instability
	if(factor[0] + factor[1] + factor[2] + factor[3] > 1.0)
	{
		// If it is small - treat instability as minor and just 'smooth' it
		// TODO - think about it more carefully
		//if( point_in_tetr(node->local_num, node->coords[0], node->coords[1], node->coords[2], tetr) )
		if(factor[0] + factor[1] + factor[2] + factor[3] < 10) // FIXME@avasyukov
		{
			if( factor[0] + factor[1] + factor[2] + factor[3] > 5.0 )
				LOG_ERROR("Factor: " << factor[0] + factor[1] + factor[2] + factor[3]);
			float sum = factor[0] + factor[1] + factor[2] + factor[3];
			for(int i = 0; i < 4; i++)
				factor[i] = factor[i] / sum;
		}
		// If point is not in tetr - throw exception
		else
		{
/*			*logger << "\tTetrVol = " < Vol;
			*logger << "\tfactor[0]=" << factor[0] << " factor[1]=" << factor[1] << " factor[2]=" << factor[2] 	<< " factor[3]=" << factor[3] << " Sum: " < factor[0] + factor[1] + factor[2] + factor[3];

			*logger << "\tnode.x[0]=" << node->coords[0] << " node.x[1]=" << node->coords[1] 
				<< " node.x[2]=" < node->coords[2];
			if( node->isFirstOrder() )
				*logger < "First order node";
			else if( node->isSecondOrder() )
				*logger < "Second order node";

			*logger << "\tv0.x[0]=" << nodes[tetr->vert[0]].coords[0] << " v0.x[1]=" << nodes[tetr->vert[0]].coords[1] << " v0.x[2]=" < nodes[tetr->vert[0]].coords[2];
						
			*logger << "\tv1.x[0]=" << nodes[tetr->vert[1]].coords[0] << " v1.x[1]=" << nodes[tetr->vert[1]].coords[1] << " v1.x[2]=" < nodes[tetr->vert[1]].coords[2];

			*logger << "\tv2.x[0]=" << nodes[tetr->vert[2]].coords[0] << " v2.x[1]=" << nodes[tetr->vert[2]].coords[1] << " v2.x[2]=" < nodes[tetr->vert[2]].coords[2];

			*logger << "\tv3.x[0]=" << nodes[tetr->vert[3]].coords[0] << " v3.x[1]=" << nodes[tetr->vert[3]].coords[1] << " v3.x[2]=" < nodes[tetr->vert[3]].coords[2];*/
			LOG_ERROR("Requested node: " << *node);
			LOG_ERROR("Node #1: " << *node0);
			LOG_ERROR("Node #2: " << *node1);
			LOG_ERROR("Node #3: " << *node2);
			LOG_ERROR("Node #4: " << *node3);
			LOG_ERROR("Factor: " << factor[0] + factor[1] + factor[2] + factor[3]);
			THROW_BAD_MESH("Sum of factors is greater than 1.0");
		}
	}

	for (int i = 0; i < 9; i++)
	{
		node->values[i] = (node0->values[i]*factor[0] 
				+ node1->values[i]*factor[1] 
				+ node2->values[i]*factor[2] 
				+ node3->values[i]*factor[3]);
	}

	node->la = (node0->la*factor[0] + node1->la*factor[1] 
		+ node2->la*factor[2] + node3->la*factor[3]);
	node->mu = (node0->mu*factor[0] + node1->mu*factor[1] 
		+ node2->mu*factor[2] + node3->mu*factor[3]);
	node->rho = (node0->rho*factor[0] + node1->rho*factor[1] 
		+ node2->rho*factor[2] + node3->rho*factor[3]);
	
	LOG_TRACE("Interpolation done");
}