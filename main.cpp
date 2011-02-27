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
	mesh->load_mesh((char*)"cube-self-gen.msh");
	TaskPreparator* tp = new TaskPreparator();
	tp->set_fixed_elastic_rheology(&(mesh->nodes), 10, 10, 10);
	SnapshotWriter* sw = new SnapshotWriter();
	sw->attach(log);
//	sw->set_basement(-4.9,-4.9,-4.9,0.98,10);
	sw->set_basement(0.1,0.1,0.1,2.44,20);
	if (sw->dump_cubic_mesh(mesh,0,0) < 0)
	{
		cout << "Can not dump!\n";
		return -1;
	}
	for(int i = 1; i < 10; i++)
	{
		cout << "Started step " << i << ". Time = " << mesh->get_current_time() << "." << endl;
		if (mesh->do_next_step() < 0)
			return -1;
		if (sw->dump_cubic_mesh(mesh,0,i) < 0)
			return -1;
		cout << "Finished step " << i << ". Time = " << mesh->get_current_time() << "." << endl;
	}	
	return 0;
}
