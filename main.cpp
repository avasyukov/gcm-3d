#include <iostream>
#include "gcm-3d.h"

using std::cout;

int main()
{
	srand( unsigned( time(0)) ) ;

	Logger* log = new Logger();
	GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis* nm = new GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis();
	//GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced* nm = new GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced();
	//GCM_Tetr_Plastic_Interpolation_1stOrder* nm = new GCM_Tetr_Plastic_Interpolation_1stOrder();
	VoidRheologyCalculator* rc = new VoidRheologyCalculator();
	TetrMesh_1stOrder* mesh = new TetrMesh_1stOrder();

	Stresser* stresser = new GCMStresser();
	stresser->loadTask("../gcm-3d-materials/data/tasks/task.xml");

	log->write(string("Mesh type: ") + *(mesh->get_mesh_type()));

	mesh->attach(log);
	mesh->attach(rc);
	mesh->attach(nm);
	mesh->attach(stresser);

	if ( mesh->load_msh_file((char*)"../gcm-3d-materials/data/models/tube.msh") < 0 )
		return -1;

//	mesh->load_gmv_file((char*)"mesh-optimized.gmv");
//	mesh->load_node_ele_files((char*)"data/models/heart.node",(char*)"data/models/heart.ele");

	TaskPreparator* tp = new TaskPreparator();
	tp->set_fixed_elastic_rheology(&(mesh->nodes), 70000, 10000, 1, 1000000);
	SnapshotWriter* sw = new SnapshotWriter();
//	sw->attach(log);
	mesh->log_mesh_quality_stats();

//	mesh->set_stress(0.0);
	sw->dump_vtk(mesh, 0, 0);

	for(int i = 1; i < 10; i++)
	{
		cout << "Started step " << i << ". Time = " << mesh->get_current_time() << "." << endl;
		for(int j = 0; j < 1; j++)
			if (mesh->do_next_step() < 0)
				return -1;
		if (sw->dump_vtk(mesh, 0, i) < 0)
			return -1;
		cout << "Finished step " << i << ". Time = " << mesh->get_current_time() << "." << endl;
	}

	return 0;
}
