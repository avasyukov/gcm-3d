#ifndef _GCM_TASK_PREPARATOR_H
#define _GCM_TASK_PREPARATOR_H  1

#ifndef TIXML_USE_STL
#define TIXML_USE_STL
#endif
#include <tinyxml.h>

#include <string>
#include <vector>

using std::string;
using std::vector;

#include "../datatypes/ElasticNode.h"
#include "../meshtypes/TetrMesh_1stOrder.h"
#include "../methods/TetrMethod_Plastic_1stOrder.h"
#include "../rheotypes/VoidRheologyCalculator.h"
#include "Logger.h"
#include "SnapshotWriter.h"
#include "VoidCollisionDetector.h"
#include "BruteforceCollisionDetector.h"
#include "TetrMeshSet.h"
#include "GCMStress.h"

class TaskPreparator
{
public:
	TaskPreparator();
	TaskPreparator(string filename);
	~TaskPreparator();
	void attach(Logger* new_logger);
	string* get_task_preparator_type();
	void set_fixed_elastic_rheology(vector<ElasticNode>* nodes, float la, float mu, float rho, float yield_limit);
	
	int load_task( string filename, string data_dir, TetrMeshSet* mesh_set );
	int load_snap_info( string filename, int* snap_num, int* step_per_snap, SnapshotWriter* sw );
protected:
	string task_filename;
	string task_preparator_type;
	Logger* logger;
};

#endif
