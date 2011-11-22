CollisionDetector::CollisionDetector()
{
	logger = NULL;
};

CollisionDetector::~CollisionDetector() { };

void CollisionDetector::attach(Logger* new_logger)
{
	logger = new_logger;
};

int CollisionDetector::find_collisions(TetrMesh_1stOrder* mesh1, TetrMesh_1stOrder* mesh2)
{
	return 0;
};
