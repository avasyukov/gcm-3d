#ifndef _GCM_DATA_BUS_H
#define _GCM_DATA_BUS_H  1

#include <string>
#include <vector>

using std::string;
using std::vector;

class DataBus;

#include "Logger.h"
#include "TetrMeshSet.h"

class DataBus
{
public:
	DataBus();
	~DataBus();
	string* get_data_bus_type();
	void attach(Logger* new_logger);
	void attach(TetrMeshSet* new_mesh_set);
	virtual float get_max_possible_tau(float local_time_step);
	virtual int sync_nodes();
protected:
	TetrMeshSet* mesh_set;
	Logger* logger;
	string data_bus_type;
};

#endif
