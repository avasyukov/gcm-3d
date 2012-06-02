#ifndef _GCM_DATA_BUS_H
#define _GCM_DATA_BUS_H  1

#ifndef TIXML_USE_STL
#define TIXML_USE_STL
#endif
#include <tinyxml.h>

#include <string>
#include <vector>

using std::string;
using std::vector;

class DataBus;

#include "CollisionDetector.h"
#include "../datatypes/Tetrahedron_1st_order.h"
#include "TetrMeshSet.h"
#include "../datatypes/MeshOutline.h"
#include <mpi.h>
#include <memory.h>

#include "GCMException.h"


class DataBus: LoggerUser
{
public:
	string* get_data_bus_type();
	void attach(CollisionDetector* cd);
	virtual float get_max_possible_tau(float local_time_step);
	// performs nodes sync
	virtual int sync_nodes();
	// loads information about zones<->processors mapping from file
	void load_zones_info(vector<int>* map);
	// returns current processor number
	int get_proc_num();
	// returns total number of processors
	int get_procs_total_num();
	// returns number of processor that owns specified zone
	int get_proc_for_zone(int zone_num);
	// gets outlines from other procs and sends local meshes outline to all
	// other procs
	void sync_outlines();
	// terminates execution
	void terminate();
	// creates custom data types
	void create_custom_types();
	// sync faces in intersection
	void sync_faces_in_intersection(MeshOutline **intersections, int **fs, int **fl);
	// sync tetrs
	void sync_tetrs();
	static DataBus* getInstance();
protected:
	DataBus();
	~DataBus();
	MPI::Errhandler error_handler;
	vector<int> **local_numbers;
	TetrMeshSet* mesh_set;
	CollisionDetector *collision_detector;
	string data_bus_type;
	int proc_num;
	int procs_total_num;
	vector<int> zones_info;

	// MPI tags
	static const int TAG_SYNC_NODE         =  2000;
	static const int TAG_SYNC_FACES_REQ_Z  =  7;
	static const int TAG_SYNC_FACES_REQ_I  =  20;
	static const int TAG_SYNC_FACES_RESP   =  19;
	static const int TAG_SYNC_FACES_F_RESP =  1000;
	static const int TAG_SYNC_FACES_N_RESP =  5000;
	static const int TAG_SYNC_TETRS_REQ    = 12;
	static const int TAG_SYNC_TETRS_REQ_I  = 19;
	static const int TAG_SYNC_TETRS_T_RESP = 13;
	static const int TAG_SYNC_TETRS_N_RESP = 14;
	static const int TAG_SYNC_TETRS_I_RESP = 21;
	static const int TAG_SYNC_NODE_TYPES   = 17;
	static const int TAG_SYNC_NODE_TYPES_I = 18;

	// MPI types
	MPI::Datatype MPI_ELNODE;
	MPI::Datatype MPI_ELNODE_NUMBERED;
	MPI::Datatype MPI_FACE_NUMBERED;
	MPI::Datatype MPI_TETR_NUMBERED;
	MPI::Datatype MPI_MESH_OUTLINE;
	MPI::Datatype MPI_OUTLINE;
	
	MPI::Datatype **MPI_NODE_TYPES;

	// return code on termination
	static const int MPI_CODE_TERMINATED = 0;
};

#endif