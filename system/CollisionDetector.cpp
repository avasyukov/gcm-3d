#include "CollisionDetector.h"

CollisionDetector::CollisionDetector()
{
	logger = NULL;
};

CollisionDetector::~CollisionDetector() { };

void CollisionDetector::attach(Logger* new_logger)
{
	logger = new_logger;
};

void CollisionDetector::set_treshold(float value)
{
	treshold = value;
};
