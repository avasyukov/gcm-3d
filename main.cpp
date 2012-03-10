#include <getopt.h>
#include <iostream>
#include <string>

#include "system/Logger.h"
#include "system/TaskPreparator.h"
#include "system/TetrMeshSet.h"
#include "system/DataBus.h"

using std::cout;

void print_help()
{
	cout << "\nUsage: gcm3d --task file --zones file --data-dir dir --result-dir dir\n" 
		<< "\t--task - xml file with task description, defaults to ./task.xml\n"
		<< "\t--zones - xml file with mapping between mesh zones and CPUs, defaults to ./zones.xml\n"
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
		c = getopt_long (argc, argv, "tzdrh", long_options, &option_index);
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
	cout << "Zones info file: " << zones_info_file << endl;
	cout << "Data dir: " << data_dir << endl;
	cout << "Res dir: " << res_dir << endl;

	// Number of snapshots in task
	int snap_num;
	// Number of steps between snapshots
	int step_per_snap;

	// Create logger to be used by all other objects
	Logger* logger = new Logger();

	// Create task preparator
	TaskPreparator* task_prep = new TaskPreparator(logger);

	// Create mesh set
	TetrMeshSet* mesh_set = new TetrMeshSet();
	mesh_set->attach(logger);

	// Create data bus
	DataBus *data_bus = new DataBus(logger);

	// Attach mesh set and data bus to each other
	mesh_set->attach(data_bus);

	// Load real task info
	task_prep->load_task( task_file, zones_info_file, data_dir, &snap_num, &step_per_snap, mesh_set, data_bus );

	// Initial nodes sync should be done before mesh preparation, because we need all nodes coordinates in place
	data_bus->sync_nodes();

	// Prepare all meshes - find borders and normals, init vectors for fast elements reverse lookups
	mesh_set->pre_process_meshes();

	// Report mesh info
	mesh_set->log_meshes_types();
	mesh_set->log_meshes_stats();

	// Create snapshot writer
	SnapshotWriter* sw = new SnapshotWriter(res_dir);

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
	delete data_bus;

	return 0;
}
