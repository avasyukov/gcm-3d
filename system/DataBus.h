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
	int local_num;
	int remote_num;
	int local_zone_num;
	int remote_zone_num;
} MPINodeRequest;

typedef struct
{
	float values[9];
	float coords[3];
	int local_num;
	int local_zone_num;
} MPINodeResponse;


// structures to hold mesh outline info during sync
typedef struct
{
	int proc_num;
	int zone_num;
} MeshOutlineInfo;

typedef struct
{
	int proc_num;
	int zone_num;
	float min_coords[3];
	float max_coords[3];
} MPIMeshOutline;

// structures to hold information while retrieving remote nodes
typedef struct
{
	int zone_num;
	float min_coords[3];
	float max_coords[3];
} MPIFacesRequest;

typedef struct
{
	int verts[3];
	int num;
	int zone_num;
} MPIFacesFResponse;

typedef struct
{
	int num;
	int zone_num;
	float coords[3];
	float values[9];
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
	// gets first message from buffer and processes it
	void process_nodes_sync_message(int source, int tag, int &nodes_to_sync, int &procs_to_sync);
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
	void sync_outlines(vector<MeshOutline> &local, vector<MeshOutline> &remote, vector<MeshOutlineInfo> &info);
	// gets first message from buffer and processes it
	int process_outlines_sync_message(int source, int tag, vector<MeshOutline> &remote, vector<MeshOutlineInfo> &info);
	// ensures that all procs are ready
	void sync();
	// retreives remote faces inside the intersection
	void get_remote_faces_in_intersection(int proc_num, int zone_num, MeshOutline &intersection, vector<ElasticNode> &remote_nodes, vector<Triangle> &remote_faces, int &procs_to_sync);
	// processes faces sync message
	bool process_faces_sync_message(int source, int tag, vector<ElasticNode> &remote_nodes, vector<Triangle> &remote_faces, int &proc_to_sync);
	// asynchronously checks for incoming messages
	bool check_messages_async(int source, int *tags, MPI::Status &status);
	// synchronously checks for incoming messages
	void check_messages_sync(int source, int *tags, MPI::Status &status);
	// notifies all procs about current proc state
	void remote_faces_sync_done();
	// terminates execution
	void terminate();
	// retreives remote tetrahedrons
	void get_remote_tetrahedrons(vector<ElasticNode> &virtual_nodes, vector<Tetrahedron_1st_order> &tetrs, vector<ElasticNode> &nodes);
	// processed a message
	void process_tetrs_sync_message(int source, int tag, vector<Tetrahedron_1st_order> &tetrs, vector<ElasticNode> &nodes, int &resps_to_get, int &procs_to_sync);
protected:
	TetrMeshSet* mesh_set;
	CollisionDetector *collision_detector;
	Logger* logger;
	string data_bus_type;
	int proc_num;
	int procs_total_num;
	vector<int> zones_info;
// FIXME
// all the following fields should be declared as protected but since we use MPI
// code outside of DataBuss class they temporary are declared as public
public:
	// MPI tags
	static const int TAG_SYNC_READY        =  0;
	static const int TAG_SYNC_TIME_STEP    =  1;
	static const int TAG_SYNC_NODE_REQ     =  2;
	static const int TAG_SYNC_NODE_RESP    =  3;
	static const int TAG_SYNC_NODE_DONE    =  4;
	static const int TAG_SYNC_OUTLINE      =  5;
	static const int TAG_SYNC_OUTLINE_DONE =  6;
	static const int TAG_SYNC_FACES_REQ    =  7;
	static const int TAG_SYNC_FACES_F_RESP =  8;
	static const int TAG_SYNC_FACES_N_RESP =  9;
	static const int TAG_SYNC_FACES_R_END  = 10;
	static const int TAG_SYNC_FACES_DONE   = 11;
	static const int TAG_SYNC_TETRS_REQ    = 12;
	static const int TAG_SYNC_TETRS_T_RESP = 13;
	static const int TAG_SYNC_TETRS_N_RESP = 14;
	static const int TAG_SYNC_TETRS_DONE   = 15;
	static const int TAG_SYNC_TETRS_R_END   = 16;

	// MPI tag classes
	// FIXME
	// google about const int* and  int const * and change the following
	// declarations to static const int
	static int TAG_CLASS_SYNC_NODE[];
	static int TAG_CLASS_SYNC_OUTLINE[];
	static int TAG_CLASS_SYNC_FACES[];
	static int TAG_CLASS_SYNC_TETRS[];

	// MPI types
	MPI::Datatype MPI_NODE_REQ;
	MPI::Datatype MPI_NODE_RESP;
	MPI::Datatype MPI_OUTLINE;
	MPI::Datatype MPI_FACES_REQ;
	MPI::Datatype MPI_TETRS_REQ;
	MPI::Datatype MPI_FACES_F_RESP;
	MPI::Datatype MPI_FACES_N_RESP;
	MPI::Datatype MPI_TETRS_T_RESP;

	// return code on termination
	static const int MPI_CODE_TERMINATED = 0;
};

#endif
