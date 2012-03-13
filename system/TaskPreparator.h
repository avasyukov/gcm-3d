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

#include "../datatypes/MeshOutline.h"
#include "../datatypes/ElasticNode.h"
#include "../meshtypes/TetrMesh_1stOrder.h"
#include "../methods/TetrMethod_Plastic_1stOrder.h"
#include "../rheotypes/VoidRheologyCalculator.h"
#include "Logger.h"
#include "SnapshotWriter.h"
#include "VoidCollisionDetector.h"
#include "BruteforceCollisionDetector.h"
#include "CollisionDetectorForLayers.h"
#include "TetrMeshSet.h"
#include "GCMStresser.h"
#include "GCMException.h"
#include "DataBus.h"

class TaskPreparator
{
public:
	TaskPreparator();
	TaskPreparator(Logger* new_logger);
	~TaskPreparator();
	void attach(Logger* new_logger);
	string* get_task_preparator_type();
	void set_fixed_elastic_rheology(vector<ElasticNode>* nodes, float la, float mu, float rho, float yield_limit);
	void set_fixed_elastic_rheology(vector<ElasticNode>* nodes, MeshOutline* box, float la, float mu, float rho, float yield_limit);
	void check_rheology_loaded(vector<ElasticNode>* nodes);
	
	int load_task( string task_file, string zones_file, string data_dir, 
				int* snap_num, int* steps_per_snap, TetrMeshSet* mesh_set, DataBus* data_bus );
protected:
	int load_zones_info(string zones_file, vector<int>* zones_info);
	string task_preparator_type;
	Logger* logger;
};

#endif
