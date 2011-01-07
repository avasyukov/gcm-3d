#include <string>
#include <vector>

#include "datatypes.h"
#include "rheotypes.h"
#include "meshtypes.h"
#include "methods.h"
#include "system.h"

//#define NDEBUG
#include <cassert>

using std::string;
using std::vector;

class MeshSet
{
	int mesh_set_num;
	vector<Mesh> meshes;
};

class SnapshotWriter
{
};

class Scheduler
{
};
