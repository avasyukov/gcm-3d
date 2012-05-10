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

#include "Logger.h"
#include "CollisionDetector.h"
#include "../datatypes/Tetrahedron_1st_order.h"
#include "TetrMeshSet.h"
#include "../datatypes/MeshOutline.h"
#include <mpi.h>
#include <memory.h>

#include "Logger.h"
#include "GCMException.h"

// structures to hold node info during sync

typedef struct
{
	int num;
	int zone_num;
	float coords[3];
	float values[9];
	float la;
	float mu;
	float rho;
} MPIFacesNResponse;

// structures to sync tetrahedrons
typedef struct
{
	int face_num;
	int zone_num;
} MPITetrsRequest;

typedef struct
{
	int verts[4];
	int zone_num;
	int face_num;
} MPITetrsTResponse;

class DataBus
{
public:
	DataBus();
	DataBus(Logger* new_logger);
	~DataBus();
	string* get_data_bus_type();
	void attach(Logger* new_logger);
	void attach(TetrMeshSet* new_mesh_set);
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
	// asynchronously checks for incoming messages
	bool check_messages_async(int source, int *tags, MPI::Status &status);
	// synchronously checks for incoming messages
	void check_messages_sync(int source, int *tags, MPI::Status &status);
	// terminates execution
	void terminate();
	// retreives remote tetrahedrons
	void get_remote_tetrahedrons(vector<ElasticNode> &virtual_nodes, vector<Tetrahedron_1st_order> &tetrs, vector<ElasticNode> &nodes);
	// processed a message
	void process_tetrs_sync_message(int source, int tag, vector<Tetrahedron_1st_order> &tetrs, vector<ElasticNode> &nodes, int &resps_to_get, int &procs_to_sync);
	// creates custom data types
	void create_custom_types();
	// sync faces in intersection
	void sync_faces_in_intersection(MeshOutline **intersections, int **fs, int **fl);
	// sync tetrs
	void sync_tetrs();
protected:
	vector<int> **local_numbers;
	TetrMeshSet* mesh_set;
	CollisionDetector *collision_detector;
	Logger* logger;
	string data_bus_type;
	int proc_num;
	int procs_total_num;
	vector<int> zones_info;

	// MPI tags
	static const int TAG_SYNC_READY        =  0;
	static const int TAG_SYNC_TIME_STEP    =  1;
	static const int TAG_SYNC_NODE         =  2000;
	static const int TAG_SYNC_OUTLINE      =  5;
	static const int TAG_SYNC_FACES_REQ_Z  =  7;
	static const int TAG_SYNC_FACES_REQ_I  =  20;
	static const int TAG_SYNC_FACES_RESP   =  19;
	static const int TAG_SYNC_FACES_F_RESP =  8;
	static const int TAG_SYNC_FACES_N_RESP =  9;
	static const int TAG_SYNC_TETRS_REQ    = 12;
	static const int TAG_SYNC_TETRS_REQ_I  = 19;
	static const int TAG_SYNC_TETRS_T_RESP = 13;
	static const int TAG_SYNC_TETRS_N_RESP = 14;
	static const int TAG_SYNC_TETRS_I_RESP = 21;
	static const int TAG_SYNC_TETRS_DONE   = 15;
	static const int TAG_SYNC_TETRS_R_END  = 16;
	static const int TAG_SYNC_NODE_TYPES   = 17;
	static const int TAG_SYNC_NODE_TYPES_I = 18;

	// MPI tag classes
	// FIXME
	// google about const int* and  int const * and change the following
	// declarations to static const int
	static int TAG_CLASS_SYNC_TETRS[];

	// MPI types
	MPI::Datatype MPI_ELNODE;
	MPI::Datatype MPI_ELNODE_NUMBERED;
	MPI::Datatype MPI_FACE;
	MPI::Datatype MPI_TETR;
	MPI::Datatype MPI_OUTLINE;
	MPI::Datatype MPI_TETRS_REQ;
	MPI::Datatype MPI_FACES_N_RESP;
	MPI::Datatype MPI_TETRS_T_RESP;
	
	MPI::Datatype **MPI_NODE_TYPES;

	// return code on termination
	static const int MPI_CODE_TERMINATED = 0;
};

#endif