#include "CollisionDetector.h"

CollisionDetector::CollisionDetector()
{
	logger = NULL;
}

CollisionDetector::~CollisionDetector() {
	//
}

void CollisionDetector::attach(Logger* new_logger)
{
	logger = new_logger;
}

void CollisionDetector::set_treshold(float value)
{
	treshold = value;
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

void CollisionDetector::find_collisions(vector<ElasticNode> &nodes, vector<ElasticNode> &border_nodes, vector<Triangle> &border_faces)
{
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
