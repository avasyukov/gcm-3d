#include <getopt.h>
#include <iostream>
#include <string>
#include "gcm-3d.h"

using std::cout;

void print_help()
{
	cout << "\nUsage: gcm3d --task file --data-dir dir --result-dir dir\n" 
		<< "\t--task - xml file with task description, defaults to ./task.xml\n"
		<< "\t--data-dir - where gcm3d will look for models specified in task.xml, defaults to ./\n"
		<< "\t--result-dir - where gcm3d will save snapshots, defaults to ./\n\n";
};

int main(int argc, char **argv)
{
	srand( unsigned( time(0)) ) ;

	// Parse command line options
	int c;
	static struct option long_options[] =
	{
		{"task"      , required_argument, 0, 't'},
		{"zones"     , required_argument, 0, 'z'},
		{"data-dir"  , required_argument, 0, 'd'},
		{"result-dir", required_argument, 0, 'r'},
		{"help"      , no_argument      , 0, 'h'},
		{0           , 0                , 0, 0  }
	};
	int option_index = 0;

	string task_file = "./task.xml";
	string zones_info_file = "./zones.xml";
	string data_dir = "./";
	string res_dir = "./";

	while ( true ) {
		c = getopt_long (argc, argv, "tdrh", long_options, &option_index);
		if (c == -1)
			break;
		switch (c)
		{
			case 't':
				task_file = optarg;
				break;
			case 'z':
				zones_info_file = optarg;
				break;
			case 'd':
				data_dir = optarg;
				break;
			case 'r':
				res_dir = optarg;
				break;
			case 'h':
				print_help();
				return 0;
			case '?':
				break;
			default:
				return -1;
		}
	}

	if(data_dir[data_dir.length()-1] != '/')
		data_dir += '/';
	if(res_dir[res_dir.length()-1] != '/')
		res_dir += '/';

	cout << "Task file: " << task_file << endl;
	cout << "Data dir: " << data_dir << endl;
	cout << "Res dir: " << res_dir << endl;

	// Prepare task
	TaskPreparator* tp = new TaskPreparator();

	// create data bus
	// TODO: if we need to be able to pass argc/argv to MPI DataBus must be
	// created before any invocation of getopt_long.
	DataBus *db = new DataBus();
	db->load_zones_info(zones_info_file);

	TetrMeshSet* mesh_set = new TetrMeshSet();
	mesh_set->attach(db);
	tp->load_task( task_file, data_dir, mesh_set );

	mesh_set->log_meshes_types();
	mesh_set->log_meshes_stats();

	int snap_num;
	int step_per_snap;
	SnapshotWriter* sw = new SnapshotWriter(res_dir);
	tp->load_snap_info( task_file, &snap_num, &step_per_snap, sw );

	// Do calculation
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

	// Delete data bus to finalize MPI
	// TODO: delete all other objects?
	delete db;

	return 0;
}
