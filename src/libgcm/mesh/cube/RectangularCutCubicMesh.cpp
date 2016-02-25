#include "libgcm/mesh/cube/RectangularCutCubicMesh.hpp"

#include <cmath>

#include "libgcm/node/CalcNode.hpp"
#include "libgcm/snapshot/VTKCubicSnapshotWriter.hpp"
#include "launcher/loaders/mesh/RectangularCutCubicMeshLoader.hpp"

using namespace gcm;
using std::numeric_limits;
using std::pair;
using std::make_pair;
using std::vector;
using std::sort;
using std::max;
using std::min;
using std::isinf;


RectangularCutCubicMesh::RectangularCutCubicMesh() :
	BasicCubicMesh(launcher::RectangularCutCubicMeshLoader::MESH_TYPE)
{
	INIT_LOGGER("gcm.RectangularCutCubicMesh");
};

void RectangularCutCubicMesh::preProcessGeometry()
{
    LOG_DEBUG("Preprocessing mesh geometry started.");
	
	for(int i = 0; i < getNodesNumber(); i++) {
		CalcNode& node = getNodeByLocalIndex(i);
		node.setIsBorder(false);
		node.setCustomFlag(1, false);
		if( !cutArea.isOutAABB(node) ) {
			node.setCustomFlag(1, true);
			node.setUsed(false);
		}
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
};

void RectangularCutCubicMesh::findBorderNodeNormal(const CalcNode& node, 
	float* x, float* y, float* z, bool debug)
{
    //CalcNode& node = getNode( border_node_index );
    assert_true(node.isBorder() );
    float normal[3];
    normal[0] = normal[1] = normal[2] = 0.0;
	uint i = node.contactDirection;
    for( int cntr = 0; cntr < 3; cntr++) {
        if( fabs(node.coords[i] - outline.min_coords[i]) < EQUALITY_TOLERANCE ) {
            normal[i] = -1;
            break;
        }
        if( fabs(node.coords[i] - outline.max_coords[i]) < EQUALITY_TOLERANCE ) {
            normal[i] = 1;
            break;
        }
		if(cutArea.isInAABB(node)) {
			if( fabs(node.coords[i] - cutArea.min_coords[i]) < EQUALITY_TOLERANCE ) {
				normal[i] = 1;
				break;
			}
			if( fabs(node.coords[i] - cutArea.max_coords[i]) < EQUALITY_TOLERANCE ) {
				normal[i] = -1;
				break;
			}
		}
		i = (i + 1) % 3;
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

void RectangularCutCubicMesh::findNearestsNodes(const vector3r& coords, int N, vector< pair<int,float> >& result)
{
	int n = 0;//floor( pow( (float)(N), 1.0 / 3.0 ) );

	int i_min =	max( int( (coords[0] - outline.minX) / meshH ) - n, 0);
	int i_max =	min( int( (coords[0] - outline.minX) / meshH ) + 1 + n, numX);
	int j_min =	max( int( (coords[1] - outline.minY) / meshH ) - n, 0);
	int j_max =	min( int( (coords[1] - outline.minY) / meshH ) + 1 + n, numY);
	int k_min =	max( int( (coords[2] - outline.minZ) / meshH ) - n, 0);
	int k_max =	min( int( (coords[2] - outline.minZ) / meshH ) + 1 + n, numZ);

	int num;
	for( int k = k_min; k <= k_max; k++ )
		for( int j = j_min; j <= j_max; j++ )
			for( int i = i_min; i <= i_max; i++ )
	        {
				num = i * (numY + 1) * (numZ + 1) + j * (numZ + 1) + k + nodes[0].number;
				CalcNode& node = getNode(num);
				result.push_back( make_pair(node.number, (coords - node.coords).length()) );
	        }
}

bool RectangularCutCubicMesh::interpolateBorderNode(real x, real y, real z,
        					real dx, real dy, real dz, CalcNode& node)
{
	// One cube
	const int N = 8;
	vector3r coords = vector3r(x + dx, y + dy, z + dz);

	if( !outline.isInAABB(coords[0], coords[1], coords[2]) || 
	    !cutArea.isOutAABB(coords[0], coords[1], coords[2]) )
		return false;
	
	if( ( !outline.isInAABB(x, y, z) ) || ( !cutArea.isOutAABB(x, y, z) ) )
	{
		vector< pair<int,float> > result;

		findNearestsNodes(coords, N, result);

		// Sorting nodes by distance
		sort(result.begin(), result.end(), sort_pred());

		for(int i = 0; i < result.size(); i++) {
			CalcNode& node1 = getNode( result[i].first );
			if( node1.isBorder() )
			{
				node = node1;
				return true;
			}
		}
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
