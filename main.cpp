#include <iostream>
#include "gcm-3d-mpi.h"

using std::cout;

int main()
{
	Logger* log = new Logger();
	NumericalMethod* nm = new NumericalMethod();
	RheologyCalculator* rc = new VoidRheologyCalculator();
	BasicTetrMesh* mesh = new BasicTetrMesh();
	log->write(string("Mesh type: ") + *(mesh->get_mesh_type()));
	mesh->attach(log);
	mesh->attach(rc);
	mesh->attach(nm);
	mesh->load_mesh((char*)"cube-small.msh");
	TaskPreparator* tp = new TaskPreparator();
	tp->set_fixed_elastic_rheology(&(mesh->nodes), 10, 10, 10);
	return 0;
}
