#ifndef _GCM_TASK_PREPARATOR_H
#define _GCM_TASK_PREPARATOR_H  1

#include <string>
#include <vector>

#include "../datatypes/ElasticNode.h"

using std::string;
using std::vector;

class TaskPreparator
{
public:
	TaskPreparator();
	~TaskPreparator();
	string* get_task_preparator_type();
	void set_fixed_elastic_rheology(vector<ElasticNode>* nodes, float la, float mu, float rho, float yield_limit);
protected:
	string task_preparator_type;
};

#include "TaskPreparator.inl"

#endif
