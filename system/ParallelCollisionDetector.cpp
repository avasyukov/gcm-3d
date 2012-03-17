#include "ParallelCollisionDetector.h"

ParallelCollisionDetector::ParallelCollisionDetector()
{
	logger = NULL;
}

void ParallelCollisionDetector::attach(Logger *logger)
{
	this->logger = logger;
}

void ParallelCollisionDetector::find_collisions(vector<ElasticNode> &nodes, vector<ElasticNode> &border_nodes, vector<Triangle> &border_faces)
{
	//
}
