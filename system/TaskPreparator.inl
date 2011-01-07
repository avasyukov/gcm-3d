TaskPreparator::TaskPreparator()
{
	task_preparator_type.assign("Generic task preparator");
};

TaskPreparator::~TaskPreparator() { };

string* TaskPreparator::get_task_preparator_type()
{
	return &task_preparator_type;
}
