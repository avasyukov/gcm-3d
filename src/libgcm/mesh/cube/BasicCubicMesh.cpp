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
	interpolator = new LineFirstOrderInterpolator();
}

gcm::BasicCubicMesh::~BasicCubicMesh()
{
	LOG_DEBUG("Destroying mesh '" << getId() << "'");
	// TODO - does it really trigger destructors?
	// TODO - do we need it here?
	nodes.clear();
	new_nodes.clear();
	delete interpolator;
	LOG_DEBUG("Mesh destroyed");
}

void gcm::BasicCubicMesh::preProcessGeometry()
{
	LOG_DEBUG("Preprocessing mesh geometry started.");
	for(int i = 0; i < getNodesNumber(); i++)
	{
		CalcNode& node = getNodeByLocalIndex(i);
		for( int i = 0; i < 3; i ++)
		{
			if( ( fabs(node.coords[i] - outline.min_coords[i]) < EQUALITY_TOLERANCE )
				|| ( fabs(node.coords[i] - outline.max_coords[i]) < EQUALITY_TOLERANCE ) )
			{
				node.setIsBorder(true);
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
	
	CalcNode& base = getNodeByLocalIndex(0);
	float h;
	
	// We suppose that mesh is uniform
	for(int i = 1; i < getNodesNumber(); i++)
	{
		CalcNode& node = getNodeByLocalIndex(i);
		h = distance(base.coords, node.coords);
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
	CalcNode& node = getNode( border_node_index );
	assert( node.isBorder() );
	float normal[3];
	normal[0] = normal[1] = normal[2] = 0.0;
	for( int i = 0; i < 3; i ++)
	{
		if( fabs(node.coords[i] - outline.min_coords[i]) < EQUALITY_TOLERANCE )
		{
			normal[i] = -1;
			break;
		}
		if( fabs(node.coords[i] - outline.max_coords[i]) < EQUALITY_TOLERANCE )
		{
			normal[i] = 1;
			break;
		}
	}
	*x = normal[0];
	*y = normal[1];
	*z = normal[2];
};

int gcm::BasicCubicMesh::findNeighbourPoint(CalcNode& node, float dx, float dy, float dz, bool debug, float* coords, bool* innerPoint)
{
	int meshSize = 1 + (outline.maxX - outline.minX + meshH * 0.1) / meshH;
	
	assert( vectorSquareNorm(dx, dy, dz) <= getMinH() * getMinH() * (1 + EQUALITY_TOLERANCE) );
	
	coords[0] = node.coords[0] + dx;
	coords[1] = node.coords[1] + dy;
	coords[2] = node.coords[2] + dz;
	
	if( !outline.isInAABB(coords[0], coords[1], coords[2]) )
	{
		//LOG_INFO("OUT");
		coords[0] = node.coords[0];
		coords[1] = node.coords[1];
		coords[2] = node.coords[2];
		*innerPoint = false;
		return -1;
	}
	
	int neighNum = node.number;
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

bool gcm::BasicCubicMesh::interpolateNode(CalcNode& origin, float dx, float dy, float dz, bool debug, 
								CalcNode& targetNode, bool& isInnerPoint)
{
	int neighInd = findNeighbourPoint( origin, dx, dy, dz, debug,
									targetNode.coords, &isInnerPoint );
	
	if( neighInd == -1 )
		return false;
	
	interpolator->interpolate( targetNode, origin, getNode( neighInd ) );
	return true;
};

bool gcm::BasicCubicMesh::interpolateNode(CalcNode& node)
{
	// Not implemented
	return false;
};