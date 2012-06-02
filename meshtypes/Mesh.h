#ifndef _GCM_MESH_H
#define _GCM_MESH_H  1

#include <string>
#include <vector>
#include <fstream>
#include <cstring>

using std::string;
using std::vector;
using std::ifstream;

#include "../datatypes/MeshOutline.h"
#include "../rheotypes/RheologyCalculator.h"
#include "../system/Stresser.h"
#include "../system/GCMException.h"

class Mesh: protected LoggerUser
{
public:
	Mesh();
	~Mesh();
	void attach(RheologyCalculator* new_rheology);
	void attach(Stresser* new_stresser);
	string* get_mesh_type();
	float get_current_time();
//	virtual int do_next_step() = 0;
	virtual float get_max_possible_tau() = 0;
	virtual int log_mesh_stats() = 0;
	virtual void translate(float x, float y, float z) = 0;

	int zone_num; // TODO do we really use it?
	int proc_num;
	
	bool local;

	MeshOutline outline;
	RheologyCalculator* rheology;

protected:
	string mesh_type;
	float current_time;
	Stresser* stresser;
};

#endif
