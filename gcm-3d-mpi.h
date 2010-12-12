#include <string>
#include <vector>

using std::string;
using std::vector;

class Element
{
	int local_num;
	int absolute_num;
};

class Tetrahedron_1st_order : Element
{
	int nodes[4];
};

class Node
{
	int zone_num;
	int local_num;
	int remote_num;
	int absolute_num;
	float coords[3];
	float fixed_coords[3];
	float values[9];
	float la;
	float mu;
	float rho;
	int elems_offset;
	int elems_size;
};

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
