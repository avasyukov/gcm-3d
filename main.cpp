#include <iostream>
#include "gcm-3d.h"

using std::cout;

int main()
{
	srand( unsigned( time(0)) ) ;

	TaskPreparator* tp = new TaskPreparator();

	TetrMeshSet* mesh_set = new TetrMeshSet();

	CollisionDetector* cd = new CollisionDetector();
	cd->set_treshold(0.1);
	mesh_set->attach(cd);

	Logger* logger = new Logger();
	mesh_set->attach(logger);

	GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis* nm = new GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis();
	//GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced* nm = new GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced();
	//GCM_Tetr_Plastic_Interpolation_1stOrder* nm = new GCM_Tetr_Plastic_Interpolation_1stOrder();
	mesh_set->attach(nm);

	VoidRheologyCalculator* rc = new VoidRheologyCalculator();
	mesh_set->attach(rc);

	Stresser* stresser = new GCMStresser();
	stresser->loadTask("../gcm-3d-materials/data/tasks/task.xml");
	mesh_set->attach(stresser);

	TetrMesh_1stOrder* mesh1 = new TetrMesh_1stOrder();
	mesh1->attach(logger);
	// if ( mesh->load_gmv_file((char*)"mesh-optimized.gmv") < 0 )
	// if ( mesh->load_node_ele_files((char*)"data/models/heart.node",(char*)"data/models/heart.ele") < 0 )
	if ( mesh1->load_msh_file((char*)"../gcm-3d-materials/data/models/cube-small.msh") < 0 ) {
		cout << "Can not open file!\n";
		return -1;
	}
	mesh1->translate(-5, 0, 0);
	tp->set_fixed_elastic_rheology(&(mesh1->nodes), 70000, 10000, 1, 1000000);
	mesh_set->attach(mesh1);

	TetrMesh_1stOrder* mesh2 = new TetrMesh_1stOrder();
	mesh2->attach(logger);
	if ( mesh2->load_msh_file((char*)"../gcm-3d-materials/data/models/sphere-small.msh") < 0 ) {
		cout << "Can not open file!\n";
		return -1;
	}
	mesh2->translate(5, 0, 0);
	tp->set_fixed_elastic_rheology(&(mesh2->nodes), 70000, 10000, 1, 1000000);
	mesh_set->attach(mesh2);

	SnapshotWriter* sw = new SnapshotWriter();
	sw->attach(logger);

	mesh_set->log_meshes_types();
	mesh_set->log_meshes_stats();

	sw->dump_vtk(mesh_set, 0);

	float cur_time;

	for(int i = 1; i < 10; i++)
	{
		if( (cur_time = mesh_set->get_current_time()) < 0)
			return -1;
		cout << "Started step " << i << ". Time = " << cur_time << "." << endl;

		for(int j = 0; j < 1; j++)
			if (mesh_set->do_next_step() < 0)
				return -1;

		if (sw->dump_vtk(mesh_set, i) < 0)
			return -1;

		if( (cur_time = mesh_set->get_current_time()) < 0)
			return -1;
		cout << "Finished step " << i << ". Time = " << cur_time << "." << endl;
	}

	return 0;
}
