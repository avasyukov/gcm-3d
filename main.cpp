#include <iostream>
#include "gcm-3d.h"

using std::cout;

int main()
{
	srand( unsigned( time(0)) ) ;

	TaskPreparator* tp = new TaskPreparator();

	TetrMeshSet* mesh_set = new TetrMeshSet();
	tp->load_task( "task.xml", mesh_set );
	
	mesh_set->log_meshes_types();
	mesh_set->log_meshes_stats();

	int snap_num;
	int step_per_snap;
	SnapshotWriter* sw = new SnapshotWriter();
	tp->load_snap_info( "task.xml", &snap_num, &step_per_snap, sw );

	float cur_time;
	for(int i = 1; i <= snap_num; i++)
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
