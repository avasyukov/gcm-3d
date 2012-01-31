#include <iostream>
#include "gcm-3d.h"

using std::cout;

int main()
{
	srand( unsigned( time(0)) ) ;

	TetrMeshSet* mesh_set = new TetrMeshSet();

	TaskPreparator* tp = new TaskPreparator();
	tp->load_task( "task.xml", mesh_set );
	
	Logger* logger = new Logger();
	SnapshotWriter* sw = new SnapshotWriter();
	sw->attach( logger );

	mesh_set->log_meshes_types();
	mesh_set->log_meshes_stats();

	sw->dump_vtk(mesh_set, 0);

	float cur_time;
	int snap_num;
	int step_per_snap;
	tp->load_snap_info( "task.xml", &snap_num, &step_per_snap );

	for(int i = 1; i < snap_num; i++)
	{
		if( (cur_time = mesh_set->get_current_time()) < 0)
			return -1;
		cout << "Started step " << i << ". Time = " << cur_time << "." << endl;

		for(int j = 0; j < step_per_snap; j++)
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
