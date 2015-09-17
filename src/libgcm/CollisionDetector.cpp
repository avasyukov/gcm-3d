#include "libgcm/CollisionDetector.hpp"

#include "libgcm/node/CalcNode.hpp"

using namespace gcm;
using std::vector;
using std::min;
using std::max;

CollisionDetector::CollisionDetector() {
    INIT_LOGGER("gcm.CollisionDetector");
    static_operation = false;
}

CollisionDetector::~CollisionDetector() {
}

void CollisionDetector::set_static(bool state)
{
    static_operation = state;
}

bool CollisionDetector::is_static()
{
    return static_operation;
}

void CollisionDetector::set_threshold(float value)
{
    threshold = value;
    LOG_DEBUG("Current threshold value: " << threshold);
}

float CollisionDetector::get_threshold()
{
    return threshold;
}

bool CollisionDetector::find_intersection(AABB &outline1, AABB &outline2, AABB &intersection)
{
    // check for intersection
    for(int j = 0; j < 3; j++) {
        intersection.min_coords[j] = fmaxf(outline1.min_coords[j] - threshold, outline2.min_coords[j] - threshold);
        intersection.max_coords[j] = fminf(outline1.max_coords[j] + threshold, outline2.max_coords[j] + threshold);
        if(intersection.min_coords[j] > intersection.max_coords[j])
            return false;
    }

    return true;
}

void CollisionDetector::find_nodes_in_intersection(Mesh* mesh, AABB& intersection, vector<CalcNode>& result)
{
    for(int i = 0; i < mesh->getNodesNumber(); i++)
    {
        CalcNode& node = mesh->getNodeByLocalIndex(i);
        // FIXME
        // only local nodes?
        if ( (node.isLocal ()) && (node.isBorder ()) )
        {
            if(intersection.isInAABB(node))
                result.push_back(node);
        }
    }
}

void CollisionDetector::find_nodes_in_intersection(BasicCubicMesh* mesh, AABB& intersection, vector<CalcNode>& result)
{
	int numX = mesh->getNumX();
	int numY = mesh->getNumY();
	int numZ = mesh->getNumZ();
	float h = mesh->getH();
	AABB outline = mesh->getOutline();

	int i_min =	max( int( (intersection.minX - outline.minX) / h ), 0);
	int i_max =	min( int( (intersection.maxX - outline.minX) / h ) + 1, numX);
	int j_min =	max( int( (intersection.minY - outline.minY) / h ), 0);
	int j_max =	min( int( (intersection.maxY - outline.minY) / h ) + 1, numY);
	int k_min =	max( int( (intersection.minZ - outline.minZ) / h ), 0);
	int k_max =	min( int( (intersection.maxZ - outline.minZ) / h ) + 1, numZ);

	int num;
	for( int k = k_min; k <= k_max; k++ )
		for( int j = j_min; j <= j_max; j++ )
			for( int i = i_min; i <= i_max; i++ )
	        {
				num = i * (numY + 1) * (numZ + 1) + j * (numZ + 1) + k;
				CalcNode& node = mesh->getNode(num);
		        if ( (node.isLocal ()) && (node.isBorder ()) )
		        {
		            if(intersection.isInAABB(node))
		                result.push_back(node);
		        }
	        }
}

void CollisionDetector::find_nodes_in_intersection(Mesh* mesh, AABB& intersection, vector<int>& result)
{
    for(int i = 0; i < mesh->getNodesNumber(); i++)
    {
        CalcNode& node = mesh->getNodeByLocalIndex(i);
        // FIXME
        // only local nodes?
        if ( (node.isLocal ()) && (node.isBorder ()) )
        {
            if(intersection.isInAABB(node))
                result.push_back(i);
        }
    }
}

/*void CollisionDetector::renumber_surface(vector<Triangle> &faces, vector<CalcNode> &nodes)
{
    if (!faces.size() || !nodes.size())
        return;
    int max_node_num = -1;
    for(int k = 0; k < nodes.size(); k++)
        if( nodes[k].local_num > max_node_num )
            max_node_num = nodes[k].local_num;
    if(max_node_num < 0)
        return;

    int *renum = new int[max_node_num + 1];
    memset(renum, 0, (max_node_num + 1) * sizeof(int));

    for(int k = 0; k < nodes.size(); k++)
        renum[ nodes[k].local_num ] = k + 1;    // +1 to avoid misinterpreting with zeroed memory

    for(int i = 0; i < faces.size(); i++) {
        for(int j = 0; j < 3; j++) {
            faces[i].vert[j] = renum[ faces[i].vert[j] ] - 1;
            if( faces[i].vert[j] < 0 )
                throw GCMException( GCMException::COLLISION_EXCEPTION, "Can not create correct numbering for surface");
        }
    }

    delete[] renum;
}

void CollisionDetector::renumber_volume(vector<Tetrahedron_1st_order> &tetrs, vector<CalcNode> &nodes)
{
    if (!tetrs.size() || !nodes.size())
        return;
    int max_node_num = -1;
    for(int k = 0; k < nodes.size(); k++)
        if( nodes[k].local_num > max_node_num )
            max_node_num = nodes[k].local_num;
    if(max_node_num < 0)
        return;

    int *renum = new int[max_node_num + 1];
    memset(renum, 0, (max_node_num + 1) * sizeof(int));

    for(int k = 0; k < nodes.size(); k++)
        renum[ nodes[k].local_num ] = k + 1;    // +1 to avoid misinterpreting with zeroed memory

    for(int i = 0; i < tetrs.size(); i++)
    {
        tetrs[i].absolute_num = tetrs[i].local_num;
        tetrs[i].local_num = i;
        for(int j = 0; j < 4; j++) {
            tetrs[i].vert[j] = renum[ tetrs[i].vert[j] ] - 1;
            if( tetrs[i].vert[j] < 0 )
                throw GCMException( GCMException::COLLISION_EXCEPTION, "Can't create correct numbering for volume");
        }
    }

    for(int i = 0; i < nodes.size(); i++)
    {
        nodes[i].absolute_num = nodes[i].local_num;
        nodes[i].local_num = i;
        nodes[i].setPlacement (Local);
    }

    delete[] renum;
}*/
