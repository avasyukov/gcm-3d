#include <iostream>
#include "gcm-3d.h"

using std::cout;

int main()
{
	Logger* log = new Logger();
	GCM_Tetr_Plastic_Interpolation_1stOrder* nm = new GCM_Tetr_Plastic_Interpolation_1stOrder();
	VoidRheologyCalculator* rc = new VoidRheologyCalculator();
	TetrMesh_1stOrder* mesh = new TetrMesh_1stOrder();
	Stresser* stresser = new Stresser();
	log->write(string("Mesh type: ") + *(mesh->get_mesh_type()));
	mesh->attach(log);
	mesh->attach(rc);
	mesh->attach(nm);
	mesh->attach(stresser);
	mesh->load_msh_file((char*)"data/models/cube-medium.msh");
//	mesh->load_gmv_file((char*)"mesh-optimized.gmv");
//	mesh->load_node_ele_files((char*)"cube.1.node",(char*)"cube.1.ele");
	TaskPreparator* tp = new TaskPreparator();
	tp->set_fixed_elastic_rheology(&(mesh->nodes), 70, 10, 10, 50);
	SnapshotWriter* sw = new SnapshotWriter();
	sw->attach(log);
	sw->set_basement(0.1,0.1,0.1,2.44,20);
	mesh->log_quality_stats();
/*
	if (sw->tmp_dump_line(mesh,0) < 0)
	{
		cout << "Can not dump!\n";
		return -1;
	}*/
	for(int i = 1; i < 10; i++)
	{
		cout << "Started step " << i << ". Time = " << mesh->get_current_time() << "." << endl;
		if (mesh->do_next_step() < 0)
			return -1;
//		if (sw->tmp_dump_line(mesh,i) < 0)
//			return -1;
		cout << "Finished step " << i << ". Time = " << mesh->get_current_time() << "." << endl;
	}
	return 0;
}
