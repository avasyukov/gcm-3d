#include "VoidCollisionDetector.h"

VoidCollisionDetector::VoidCollisionDetector()
{
	logger = NULL;
};

VoidCollisionDetector::~VoidCollisionDetector() { };
/*
void VoidCollisionDetector::attach(Logger* new_logger)
{
	logger = new_logger;
};

void VoidCollisionDetector::set_treshold(float value)
{
	treshold = value;
};
*/
int VoidCollisionDetector::find_collisions(TetrMesh_1stOrder* mesh1, TetrMesh_1stOrder* mesh2, vector<ElasticNode>* virt_nodes, float time_step)
{
	return 0;
};
