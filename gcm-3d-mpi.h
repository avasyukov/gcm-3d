#include <string>
#include <vector>

#include "datatypes.h"

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

class Logger
{
};

class Scheduler
{
};

class NumericalMethod
{
};

class RheologyCalculator
{
};

class TaskPreparator
{
};
