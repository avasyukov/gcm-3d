#include "libgcm/mesh/cube/BasicCubicMesh.hpp"

#include "libgcm/node/CalcNode.hpp"
#include "libgcm/snapshot/VTKCubicSnapshotWriter.hpp"

using namespace gcm;
using std::numeric_limits;

BasicCubicMesh::BasicCubicMesh()
{
    meshH = numeric_limits<float>::infinity();
    // FIXME - hardcoded name
    numericalMethodType = "InterpolationFixedAxis";
    // FIXME - hardcoded parameter
    numericalMethodOrder = 1;
    INIT_LOGGER("gcm.BasicCubicMesh");
    LOG_DEBUG("Creating mesh");
    interpolator1 = new LineFirstOrderInterpolator();
    interpolator2 = new LineSecondOrderInterpolator();
}

BasicCubicMesh::~BasicCubicMesh()
{
    LOG_DEBUG("Destroying mesh '" << getId() << "'");
    // TODO - does it really trigger destructors?
    // TODO - do we need it here?
    nodes.clear();
    new_nodes.clear();
    delete interpolator1;
    delete interpolator2;
    LOG_DEBUG("Mesh destroyed");
}

void BasicCubicMesh::preProcessGeometry()
{
    LOG_DEBUG("Preprocessing mesh geometry started.");
	for(int i = 0; i < getNodesNumber(); i++)
		getNodeByLocalIndex(i).setIsBorder(false);
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

void BasicCubicMesh::doNextPartStep(float tau, int stage)
{
    defaultNextPartStep(tau, stage);
};

void BasicCubicMesh::logMeshStats()
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

void BasicCubicMesh::checkTopology(float tau)
{
    // We do not need it for this mesh type
}

float BasicCubicMesh::getRecommendedTimeStep()
{
    return getMinH() / getMaxEigenvalue();
};

void BasicCubicMesh::calcMinH()
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

float BasicCubicMesh::getAvgH() {
	if( isinf( meshH ) )
        calcMinH();
    return meshH;
};

float BasicCubicMesh::getMinH()
{
    if( isinf( meshH ) )
        calcMinH();
    return meshH;
};

void BasicCubicMesh::findBorderNodeNormal(const CalcNode& node, float* x, float* y, float* z, bool debug)
{
    //CalcNode& node = getNode( border_node_index );
    assert_true(node.isBorder() );
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

int BasicCubicMesh::findNeighbourPoint(CalcNode& node, float dx, float dy, float dz,
	bool debug, float* coords, bool* innerPoint)
{
    //int meshSizeX = 1 + (outline.maxX - outline.minX + meshH * 0.1) / meshH;
	int meshSizeY = 1 + (outline.maxY - outline.minY + meshH * 0.1) / meshH;
	int meshSizeZ = 1 + (outline.maxZ - outline.minZ + meshH * 0.1) / meshH;

    assert_le(vectorSquareNorm(dx, dy, dz), getMinH() * getMinH() * (1 + EQUALITY_TOLERANCE) );

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
        neighNum += meshSizeY*meshSizeZ;
    else if ( dx < -EQUALITY_TOLERANCE )
        neighNum -= meshSizeY*meshSizeZ;
    else if ( dy > EQUALITY_TOLERANCE )
        neighNum += meshSizeZ;
    else if ( dy < -EQUALITY_TOLERANCE )
        neighNum -= meshSizeZ;
    else if ( dz > EQUALITY_TOLERANCE )
        neighNum += 1;
    else if ( dz < -EQUALITY_TOLERANCE )
        neighNum -= 1;

    *innerPoint = true;
    return neighNum;
};

bool BasicCubicMesh::interpolateNode(CalcNode& origin, float dx, float dy, float dz, bool debug,
                                CalcNode& targetNode, bool& isInnerPoint)
{
    int neighInd = findNeighbourPoint( origin, dx, dy, dz, debug,
                                    targetNode.coords, &isInnerPoint );

    if( neighInd == -1 )
        return false;

    CalcNode tmpNode;
    int secondNeighInd = findNeighbourPoint( origin, -dx, -dy, -dz, debug,
                                    tmpNode.coords, &isInnerPoint );

    //interpolator1->interpolate( targetNode, origin, getNode( neighInd ) );
    //return true;
    
    if( secondNeighInd == -1 )
    {
        interpolator1->interpolate( targetNode, origin, getNode( neighInd ) );
    }
    else
    {
        int leftInd, rightInd;
        if( dx + dy + dz > 0 )
        {
            leftInd = secondNeighInd;
            rightInd = neighInd;
        }
        else
        {
            rightInd = secondNeighInd;
            leftInd = neighInd;
        }
        interpolator2->interpolate( targetNode, getNode( leftInd ), origin, getNode( rightInd ) );
    }
    return true;
};

bool BasicCubicMesh::interpolateNode(CalcNode& node)
{
    // Not implemented
    return false;
};

bool BasicCubicMesh::interpolateBorderNode(real x, real y, real z, 
                                real dx, real dy, real dz, CalcNode& node)
{
    //int meshSizeX = 1 + (outline.maxX - outline.minX + meshH * 0.1) / meshH;
    float coords[3];
    float tx = coords[0] = x + dx;
    float ty = coords[1] = y + dy;
    float tz = coords[2] = z + dz;

    if( outline.isInAABB(tx, ty, tz) != outline.isInAABB(x, y, z) )
    {
        // FIXME_ASAP
        float minH = distance(coords, getNodeByLocalIndex(0).coords);
        int num = 0;
        for(int i = 1; i < getNodesNumber(); i++)
        {
            CalcNode& node = getNodeByLocalIndex(i);
            float h = distance(coords, node.coords);
            if( h < minH )
            {
                minH = h;
                num = i;
            }
        }
        node = getNodeByLocalIndex(num);
        
        return true;
    }
	
    return false;
};

const SnapshotWriter& BasicCubicMesh::getSnaphotter() const
{
    return VTKCubicSnapshotWriter::getInstance();
}

const SnapshotWriter& BasicCubicMesh::getDumper() const
{
    return getSnaphotter();
}
