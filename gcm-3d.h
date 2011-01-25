#include <string>
#include <vector>

#include "system.h"
#include "datatypes.h"
#include "rheotypes.h"
#include "meshtypes.h"
#include "methods.h"

//#define NDEBUG
#include <cassert>

using std::string;
using std::vector;

class MeshSet
{
	int mesh_set_num;
	vector<Mesh> meshes;
};

class Scheduler
{
};
