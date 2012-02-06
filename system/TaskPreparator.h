#ifndef _GCM_TASK_PREPARATOR_H
#define _GCM_TASK_PREPARATOR_H  1

#include <string>
#include <vector>

#include "../meshtypes/TetrMesh_1stOrder.h"
#include "../datatypes/ElasticNode.h"
#include "../system/Logger.h"
#include "SnapshotWriter.h"
#include "CollisionDetector.h"
#include "../system/TetrMeshSet.h"
#include "../methods/GCM_Tetr_Elastic_Interpolation_1stOrder.h"
#include "../methods/GCM_Tetr_Plastic_Interpolation_1stOrder.h"
#include "../methods/GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced.h"
#include "../methods/GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis.h"

using std::string;
using std::vector;

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

#include "TaskPreparator.inl"

#endif
