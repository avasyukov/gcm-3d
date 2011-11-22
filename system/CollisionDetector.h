#ifndef _GCM_COLLISION_DETECTOR
#define _GCM_COLLISION_DETECTOR 1

#include <vector>
#include <string>

using std::string;
using std::vector;

class CollisionDetector
{
public:
	CollisionDetector();
	~CollisionDetector();
	void attach(Logger* new_logger);
	int find_collisions(TetrMesh_1stOrder* mesh1, TetrMesh_1stOrder* mesh2);
protected:
	Logger* logger;
};

#include "CollisionDetector.inl"

#endif
