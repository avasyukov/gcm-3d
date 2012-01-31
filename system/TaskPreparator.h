#ifndef _GCM_TASK_PREPARATOR_H
#define _GCM_TASK_PREPARATOR_H  1

#include <string>
#include <vector>

#include "../meshtypes/TetrMesh_1stOrder.h"
#include "../datatypes/ElasticNode.h"
#include "../system/Logger.h"
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
	TaskPreparator(string fname);
	~TaskPreparator();
	string* get_task_preparator_type();
	void set_fixed_elastic_rheology(vector<ElasticNode>* nodes, float la, float mu, float rho, float yield_limit);
	
	int load_task( string fname, TetrMeshSet* mesh_set );
	int load_snap_info( string fname, int* snap_num, int* step_per_snap );
protected:
	string task_fname;
	string task_preparator_type;
};

#include "TaskPreparator.inl"

#endif
