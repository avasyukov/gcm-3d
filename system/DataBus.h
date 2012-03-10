#ifndef _GCM_DATA_BUS_H
#define _GCM_DATA_BUS_H  1

#include <string>
#include <vector>
#include <sstream>

using std::string;
using std::vector;
using std::stringstream;

class DataBus;

#include "Logger.h"
#include "TetrMeshSet.h"
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

class DataBus
{
public:
	DataBus();
	~DataBus();
	string* get_data_bus_type();
	void attach(Logger* new_logger);
	void attach(TetrMeshSet* new_mesh_set);
	virtual float get_max_possible_tau(float local_time_step);
	// performs nodes sync
	virtual int sync_nodes();
	// gets first message from buffer and processes it
	int process_request(int source, int tag);
	// loads information about zones<->processors mapping from file
	void load_zones_info(string file_name);
	// returns current processor number
	int get_proc_num();
	// returns total number of processors
	int get_total_proc_num();
	// returns number of processor that owns specified zone
	int get_proc_for_zone(int zone_num);
protected:
	TetrMeshSet* mesh_set;
	Logger* logger;
	string data_bus_type;
	int proc_num;
	int proc_total_num;
	vector<int> zones_info;

	// MPI tags
	static const int TAG_SYNC_TIME_STEP = 1;
	static const int TAG_SYNC_NODE_REQ  = 2;
	static const int TAG_SYNC_NODE_RESP = 3;
	static const int TAG_SYNC_NODE_DONE = 4;

	// MPI types
	MPI::Datatype MPI_NODE_REQ;
	MPI::Datatype MPI_NODE_RESP;
};

#endif
