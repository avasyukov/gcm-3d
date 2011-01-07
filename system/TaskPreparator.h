#ifndef _GCM_TASK_PREPARATOR_H
#define _GCM_TASK_PREPARATOR_H  1

#include <string>
#include <vector>

using std::string;
using std::vector;

class TaskPreparator
{
public:
	TaskPreparator();
	~TaskPreparator();
	string* get_task_preparator_type();
protected:
	string task_preparator_type;
};

#include "TaskPreparator.inl"

#endif
