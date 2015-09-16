#include "libgcm/mesh/cube/RectangularCutCubicMesh.hpp"

#include "libgcm/node/CalcNode.hpp"

using namespace gcm;
using std::numeric_limits;

RectangularCutCubicMesh::RectangularCutCubicMesh() :
	BasicCubicMesh()
{
	INIT_LOGGER("gcm.RectangularCutCubicMesh");
};

void RectangularCutCubicMesh::doNextPartStep(float tau, int stage) {
	defaultNextPartStep(tau, stage);
	for(int i = 0; i < getNodesNumber(); i++) {
		CalcNode& node = getNodeByLocalIndex(i);
		if( !cutArea.isOutAABB(node) ) {
			for(int i = 0; i < 9; i++)
				node.values[i] = 0;
		}
	}
};

void RectangularCutCubicMesh::preProcessGeometry()
{
    LOG_DEBUG("Preprocessing mesh geometry started.");
	
	for(int i = 0; i < getNodesNumber(); i++) {
		CalcNode& node = getNodeByLocalIndex(i);
		node.setIsBorder(false);
		node.setCustomFlag(1, false);
		if( !cutArea.isOutAABB(node) )
			node.setCustomFlag(1, true);
	}
	
    // for usual AABB outline
	for(int i = 0; i < getNodesNumber(); i++)
    {
        CalcNode& node = getNodeByLocalIndex(i);
        for( int i = 0; i < 3; i ++)
        {
            if( ( fabs(node.coords[i] - outline.min_coords[i]) < EQUALITY_TOLERANCE )
                || ( fabs(node.coords[i] - outline.max_coords[i]) < EQUALITY_TOLERANCE ) )
            {
                node.setIsBorder(true); break;
            }
        }
    }
	// for cutArea
	for(int i = 0; i < getNodesNumber(); i++)
    {
        CalcNode& node = getNodeByLocalIndex(i);
		if(cutArea.isInAABB(node)) {
			for( int i = 0; i < 3; i ++)
				if( ( fabs(node.coords[i] - cutArea.min_coords[i]) < EQUALITY_TOLERANCE )
					|| ( fabs(node.coords[i] - cutArea.max_coords[i]) < EQUALITY_TOLERANCE ) )
				{
					node.setIsBorder(true); break;
				}
		}
	}
	
    LOG_DEBUG("Preprocessing mesh geometry done.");
}

void RectangularCutCubicMesh::findBorderNodeNormal(const CalcNode& node, float* x, float* y, float* z, bool debug)
{
    //CalcNode& node = getNode( border_node_index );
    assert_true(node.isBorder() );
    float normal[3];
    normal[0] = normal[1] = normal[2] = 0.0;
    for( int i = 0; i < 3; i++) {
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
		if( fabs(node.coords[i] - cutArea.min_coords[i]) < EQUALITY_TOLERANCE )
        {
            normal[i] = 1;
            break;
        }
        if( fabs(node.coords[i] - cutArea.max_coords[i]) < EQUALITY_TOLERANCE )
        {
            normal[i] = -1;
            break;
        }
    }
    *x = normal[0];
    *y = normal[1];
    *z = normal[2];
};

int RectangularCutCubicMesh::findNeighbourPoint(CalcNode& node, float dx, float dy, float dz,
	bool debug, float* coords, bool* innerPoint)
{
    //int meshSizeX = 1 + (outline.maxX - outline.minX + meshH * 0.1) / meshH;
	int meshSizeY = 1 + (outline.maxY - outline.minY + meshH * 0.1) / meshH;
	int meshSizeZ = 1 + (outline.maxZ - outline.minZ + meshH * 0.1) / meshH;

    assert_le(vectorSquareNorm(dx, dy, dz), getMinH() * getMinH() * (1 + EQUALITY_TOLERANCE) );

    coords[0] = node.coords[0] + dx;
    coords[1] = node.coords[1] + dy;
    coords[2] = node.coords[2] + dz;

    if( !outline.isInAABB(coords[0], coords[1], coords[2]) || 
		!cutArea.isOutAABB(coords[0], coords[1], coords[2]) )
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

bool RectangularCutCubicMesh::interpolateBorderNode(real x, real y, real z, 
                                real dx, real dy, real dz, CalcNode& node)
{
    //int meshSizeX = 1 + (outline.maxX - outline.minX + meshH * 0.1) / meshH;
    float coords[3];
    float tx = coords[0] = x + dx;
    float ty = coords[1] = y + dy;
    float tz = coords[2] = z + dz;

    if( (outline.isInAABB(tx, ty, tz) != outline.isInAABB(x, y, z)) ||
		(cutArea.isOutAABB(tx, ty, tz) != cutArea.isOutAABB(x, y, z)) )
    {
        // FIXME_ASAP
        float minH = std::numeric_limits<float>::infinity();
        int num = -1;
        for(int i = 1; i < getNodesNumber(); i++) {
            CalcNode& node = getNodeByLocalIndex(i);
			if(node.isBorder()) {
				float h = distance(coords, node.coords);
				if( h < minH ) {
					minH = h;
					num = i;
				}
			}
        }
        node = getNodeByLocalIndex(num);
        
        return true;
    }
	
    return false;
};

void RectangularCutCubicMesh::transfer(float x, float y, float z) {
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
	if( !isinf(cutArea.minX) )
    {
        cutArea.transfer(x, y, z);
    }
	
	
    // TODO@avasyukov - think about additional checks
    Engine::getInstance().transferScene(x, y, z);
};
