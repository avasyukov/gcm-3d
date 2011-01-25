#include <iostream>
#include "gcm-3d.h"

using std::cout;

int main()
{
	Logger* log = new Logger();
	GCM_Tetr_Elastic_Interpolation_1stOrder* nm = new GCM_Tetr_Elastic_Interpolation_1stOrder();
	VoidRheologyCalculator* rc = new VoidRheologyCalculator();
	TetrMesh_1stOrder* mesh = new TetrMesh_1stOrder();
	Stresser* stresser = new Stresser();
	log->write(string("Mesh type: ") + *(mesh->get_mesh_type()));
	mesh->attach(log);
	mesh->attach(rc);
	mesh->attach(nm);
	mesh->attach(stresser);
	mesh->load_mesh((char*)"cube-small.msh");
	TaskPreparator* tp = new TaskPreparator();
	tp->set_fixed_elastic_rheology(&(mesh->nodes), 10, 10, 10);
	mesh->do_next_step();
	return 0;
}
