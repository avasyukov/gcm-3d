#ifndef _GCM_DATA_BUS_H
#define _GCM_DATA_BUS_H  1

#include <string>
#include <vector>

using std::string;
using std::vector;

#include "Logger.h"

class DataBus
{
public:
	DataBus();
	~DataBus();
	string* get_data_bus_type();
	void attach(Logger* new_logger);
	virtual float get_max_possible_tau(float local_time_step);
	virtual int sync_nodes();
protected:
	Logger* logger;
	string data_bus_type;
};

#include "DataBus.inl"

#endif
