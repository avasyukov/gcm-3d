#ifndef _GCM_MESH_SET
#define _GCM_MESH_SET 1

#include <vector>
#include <string>

using std::vector;
using std::string;

#include "../meshtypes/TetrMesh_1stOrder.h"
#include "../methods/TetrNumericalMethod.h"
#include "../rheotypes/RheologyCalculator.h"
#include "DataBus.h"
#include "Stresser.h"
#include "CollisionDetector.h"

class TetrMeshSet: LoggerUser
{
public:
	void init();
	static TetrMeshSet* getInstance();
	void attach(Stresser* new_stresser);
	void attach(RheologyCalculator* new_rheology);
	void attach(TetrNumericalMethod* new_numerical_method);
	void attach(TetrMesh_1stOrder* new_mesh);
	void attach(CollisionDetector* new_collision_detector);
	void log_meshes_types();
	void log_meshes_stats();
	float get_current_time();
	int do_next_step();
	int get_number_of_meshes();
	int get_number_of_local_meshes();
	int get_number_of_remote_meshes();
	// Prepares loaded meshes for calculation - finds normals and borders, created reverse lookup vectors, etc
	// Should be called after remote nodes are synced
	void pre_process_meshes();
	// Returns max possible tau for all attached meshes
	float get_max_possible_tau();
	TetrMesh_1stOrder* get_mesh(int num);
	TetrMesh_1stOrder* get_local_mesh(int num);
	TetrMesh_1stOrder* get_remote_mesh(int num);
	TetrMesh_1stOrder* get_mesh_by_zone_num(int zone_num);
	ElasticNode* getNode(int num);
	
	void init_mesh_container(vector<int> &zones_info);	
	void sync_remote_data();
protected:
	TetrMeshSet();
	~TetrMeshSet();
	RheologyCalculator* rheology;
	DataBus* data_bus;
	Stresser* stresser;
	TetrNumericalMethod* numerical_method;
	CollisionDetector* collision_detector;
	TetrMesh_1stOrder* meshes;
	vector<TetrMesh_1stOrder*> local_meshes;
	vector<TetrMesh_1stOrder*> remote_meshes;
public:
	// FIXME
	// it's public for a while :)
	vector<ElasticNode> virt_nodes;
	// FIXME
	int meshes_number;
	int *meshes_at_proc;
};

#endif
