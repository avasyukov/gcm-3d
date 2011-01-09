#ifndef _GCM_MESH_H
#define _GCM_MESH_H  1

#include <string>
#include <vector>
#include <fstream>
#include <cstring>

using std::ifstream;

#include "../system/Logger.h"
#include "../rheotypes/RheologyCalculator.h"
#include "../methods/NumericalMethod.h"
#include "../system/DataBus.h"

using std::string;
using std::vector;

class Mesh
{
public:
	Mesh();
	~Mesh();
	void attach(Logger* new_logger);
	void attach(NumericalMethod* new_numerical_method);
	void attach(RheologyCalculator* new_rheology);
	void attach(DataBus* new_data_bus); // TODO what should be attach to what?
	string* get_mesh_type();
	float get_current_time();
	virtual int do_next_step() = 0;
	virtual float get_max_possible_tau() = 0;

	int zone_num;
	int mesh_num;

protected:
	string mesh_type;
	float current_time;
	Logger* logger;
	RheologyCalculator* rheology;
	NumericalMethod* method;
	DataBus* data_bus; // TODO implement at least draft structure
};

#include "Mesh.inl"

#endif
