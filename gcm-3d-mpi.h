#include <string>
#include <vector>

#include "datatypes.h"
#include "methods.h"
#include "system.h"
#include "rheology.h"
#include "grids.h"

using std::string;
using std::vector;

class Mesh
{
	int zone_num;
	string mesh_type;
	string num_method_type;
	string rheo_type;
	vector<Node> nodes;
	vector<Element> elems;
};

class MeshSet
{
	int mesh_set_num;
	vector<Mesh> meshes;
};

class Stresser
{
};

class SnapshotWriter
{
};

class Scheduler
{
};

class TaskPreparator
{
};
