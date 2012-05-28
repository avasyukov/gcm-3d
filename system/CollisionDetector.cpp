#include "CollisionDetector.h"

CollisionDetector::CollisionDetector()
{
	logger = NULL;
	data_bus = NULL;
	mesh_set = NULL;
}

CollisionDetector::~CollisionDetector() {
	//
}

void CollisionDetector::attach(Logger* new_logger)
{
	logger = new_logger;
}

void CollisionDetector::attach(DataBus* new_data_bus)
{
	data_bus = new_data_bus;
}

void CollisionDetector::attach(TetrMeshSet* new_mesh_set)
{
	mesh_set = new_mesh_set;
}

void CollisionDetector::set_treshold(float value)
{
	treshold = value;
}

float CollisionDetector::get_treshold()
{
	return treshold;
}

bool CollisionDetector::find_intersection(MeshOutline &outline1, MeshOutline &outline2, MeshOutline &intersection)
{

	// check for intersection
	for(int j = 0; j < 3; j++) {
		intersection.min_coords[j] = fmaxf(outline1.min_coords[j] - treshold, outline2.min_coords[j] - treshold);
		intersection.max_coords[j] = fminf(outline1.max_coords[j] + treshold, outline2.max_coords[j] + treshold);
		if(intersection.min_coords[j] > intersection.max_coords[j])
			return false;
	}
	
	return true;

}

void CollisionDetector::find_nodes_in_intersection(vector<ElasticNode> &nodes, MeshOutline &intersection, vector<ElasticNode> &result)
{
	for (int i = 0; i < nodes.size(); i++)
		// FIXME
		// only local nodes?
		if (nodes[i].placement_type == LOCAL)
		{
			bool flag = true;
			for (int j = 0; j < 3; j++)
				if (nodes[i].coords[j] < intersection.min_coords[j] || nodes[i].coords[j] > intersection.max_coords[j])
				{
					flag = false;
					break;
				}
			if (flag)
				result.push_back(nodes[i]);
		}
}

void CollisionDetector::find_nodes_in_intersection(vector<ElasticNode> &nodes, MeshOutline &intersection, vector<int> &result)
{
	for (int i = 0; i < nodes.size(); i++)
		// FIXME
		// only local nodes?
		if (nodes[i].placement_type == LOCAL)
		{
			bool flag = true;
			for (int j = 0; j < 3; j++)
				if (nodes[i].coords[j] < intersection.min_coords[j] || nodes[i].coords[j] > intersection.max_coords[j])
				{
					flag = false;
					break;
				}
			if (flag)
				result.push_back(i);
		}
}

void CollisionDetector::find_faces_in_intersection(vector<Triangle> &faces, vector<ElasticNode> &nodes, MeshOutline &intersection, vector<Triangle> &result)
{
	for (int i = 0; i < faces.size(); i++)
	{
		int verts = 0;
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 3; k++)
				if (nodes[faces[i].vert[j]].coords[k] < intersection.min_coords[k] || nodes[faces[i].vert[j]].coords[k] > intersection.max_coords[k])
				{
					verts++;
					break;
				}
		if (verts != 3)
			result.push_back(faces[i]);
	}
}

void CollisionDetector::find_faces_in_intersection(vector<Triangle> &faces, vector<ElasticNode> &nodes, MeshOutline &intersection, vector<int> &result)
{
	for (int i = 0; i < faces.size(); i++)
	{
		int verts = 0;
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 3; k++)
				if (nodes[faces[i].vert[j]].coords[k] < intersection.min_coords[k] || nodes[faces[i].vert[j]].coords[k] > intersection.max_coords[k])
				{
					verts++;
					break;
				}
		if (verts != 3)
			result.push_back(i);
	}
}

void CollisionDetector::renumber_surface(vector<Triangle> &faces, vector<ElasticNode> &nodes)
{
	int max_node_num = -1;
	for(int k = 0; k < nodes.size(); k++)
		if( nodes[k].local_num > max_node_num )
			max_node_num = nodes[k].local_num;

	int *renum = (int*) malloc( max_node_num * sizeof(int) );
	memset(renum, 0, max_node_num * sizeof(int));

	for(int k = 0; k < nodes.size(); k++)
		renum[ nodes[k].local_num ] = k + 1;	// +1 to avoid misinterpreting with zeroed memory

	for(int i = 0; i < faces.size(); i++) {
		for(int j = 0; j < 3; j++) {
			faces[i].vert[j] = renum[ faces[i].vert[j] ] - 1;
			if( faces[i].vert[j] < 0 )
				throw GCMException( GCMException::COLLISION_EXCEPTION, "Can not create correct numbering for surface");
		}
	}

	free(renum);
}

void CollisionDetector::renumber_volume(vector<Tetrahedron_1st_order> &tetrs, vector<ElasticNode> &nodes)
{
	int max_node_num = -1;
	for(int k = 0; k < nodes.size(); k++)
		if( nodes[k].local_num > max_node_num )
			max_node_num = nodes[k].local_num;

	int *renum = (int*) malloc( max_node_num * sizeof(int) );
	memset(renum, 0, max_node_num * sizeof(int));

	for(int k = 0; k < nodes.size(); k++)
		renum[ nodes[k].local_num ] = k + 1;	// +1 to avoid misinterpreting with zeroed memory

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
		nodes[i].placement_type = LOCAL;
	}

	free(renum);
}
