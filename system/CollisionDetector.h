#ifndef _GCM_COLLISION_DETECTOR
#define _GCM_COLLISION_DETECTOR 1

#include <vector>
#include <string>

using std::vector;
using std::string;

class CollisionDetector;

#include "../datatypes/ElasticNode.h"
#include "../meshtypes/TetrMesh_1stOrder.h"
#include "Logger.h"
#include "GCMException.h"
#include "DataBus.h"
#include "TetrMeshSet.h"

class CollisionDetector
{
public:
	CollisionDetector();
	~CollisionDetector();
	void set_treshold(float value);
	float get_treshold();
	void attach(Logger* new_logger);
	void attach(DataBus* new_data_bus);
	void attach(TetrMeshSet* new_mesh_set);
	virtual void find_collisions(vector<ElasticNode> &virt_nodes) = 0;
	// return elements that are in intersection
	void find_nodes_in_intersection(vector<ElasticNode> &nodes, MeshOutline &intersection, vector<ElasticNode> &result);
	void find_faces_in_intersection(vector<Triangle> &faces, vector<ElasticNode> &nodes, MeshOutline &intersection, vector<Triangle> &result);
	void find_faces_in_intersection(vector<Triangle> &faces, vector<ElasticNode> &nodes, MeshOutline &intersection, vector<int> &result);
	// number returned surface elements (nodes and triangles) sequentially
	void renumber_surface(vector<Triangle> &faces, vector<ElasticNode> &nodes);
	// returns surface elements (nodes and triangles) renumbered sequentially
	void renumber_volume(vector<Tetrahedron_1st_order> &tetrs, vector<ElasticNode> &nodes);
	// finds collisions between nodes and faces 
	// FIXME
	// it seems this function is not ElasticNode-specific
	// void find_collisions(vector<ElasticNode> &nodes, vector<ElasticNode> &border_nodes, vector<Triangle> &border_faces);
	// checks two outlines for intersection, returns true if found
	bool find_intersection(MeshOutline &outline1, MeshOutline &outline2, MeshOutline &intersection);
protected:
	Logger* logger;
	DataBus* data_bus;
	TetrMeshSet* mesh_set;
	float treshold;
};

#endif
