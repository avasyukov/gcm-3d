#include <getopt.h>
#include <iostream>
#include <string>

#include "system/Logger.h"
#include "system/TaskPreparator.h"
#include "system/TetrMeshSet.h"
#include "system/DataBus.h"
#include "system/GCMException.h"
#include "system/LineSnapshotWriter.h"

using std::cout;

// This function print usage message
void print_help()
{
	cout << "\nUsage: gcm3d --task file --zones file --data-dir dir --result-dir dir --log-file log\n" 
		<< "\t--task - xml file with task description, defaults to ./task.xml\n"
		<< "\t--zones - xml file with mapping between mesh zones and CPUs, defaults to ./zones.xml\n"
		<< "\t--data-dir - where gcm3d will look for models specified in task.xml, defaults to ./\n"
		<< "\t--log-file - file to write all output, defaults to stdout\n";
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
		{"log-file"  , required_argument, 0, 'l'},
		{"dump-vtk"  , required_argument, 0, 'D'},
		{"dump-line" , required_argument, 0, 'L'},
		{"help"      , no_argument      , 0, 'h'},
		{0           , 0                , 0, 0  }
	};
	int option_index = 0;

	string task_file = "./task.xml";
	string zones_info_file = "./zones.xml";
	string data_dir = "./";
	
	vector<SnapshotWriter*> sw;

	// Top level objects
	Logger* logger = Logger::getInstace();
	TaskPreparator* task_prep = NULL;
	TetrMeshSet* mesh_set = TetrMeshSet::getInstance();
	DataBus *data_bus = DataBus::getInstance();
	mesh_set->init();	
	
	// suppress error messages
	opterr = 0;
	SnapshotWriter *s;
	while ((c = getopt_long (argc, argv, "t:z:d:l:hD:L:", long_options, &option_index)) != -1)
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
			case 'h':
				print_help();
				return 0;
			case 'l':
				logger->setFileOutput(optarg);
				break;
			case 'D':
				s = new VTKSnapshotWriter(optarg);
				sw.push_back(s);
				s->parseArgs(argc, argv);
				s->init();
				break;
			case 'L':
				s = new LineSnapshotWriter(optarg);
				sw.push_back(s);
				s->parseArgs(argc, argv);
				s->init();
				break;				
			case '?':
				print_help();
			default:
				return -1;
		}
		
	logger->init();

	if(data_dir[data_dir.length()-1] != '/')
		data_dir += '/';

	// Number of snapshots in task
	int snap_num = -1;
	// Number of steps between snapshots
	int step_per_snap = -1;
	try
	{
		// Create task preparator
		task_prep = new TaskPreparator();
		
		*logger << "Task file: " < task_file;
		*logger << "Zones info file: " < zones_info_file;
		*logger << "Data dir: " < data_dir;

		// Load real task info
		task_prep->load_task( task_file, zones_info_file, data_dir, &snap_num, &step_per_snap);
		
		// create custom types for fast sync
		data_bus->create_custom_types();

		// Initial nodes sync should be done before mesh preparation, because we need all nodes coordinates in place
		data_bus->sync_nodes();

		// Prepare all meshes - find borders and normals, init vectors for fast elements reverse lookups
		mesh_set->pre_process_meshes();

		// Report mesh info
		mesh_set->log_meshes_types();
		mesh_set->log_meshes_stats();

		for (int i = 0; i < sw.size(); i++)
			sw[i]->dump(0);

		// Do calculation
		float cur_time;
		for(int i = 1; i <= snap_num; i++)
		{
			if( (cur_time = mesh_set->get_current_time()) < 0)
				return -1;
			*logger << "Started step " << i << ". Time = " << cur_time < ".";

			for(int j = 0; j < step_per_snap; j++)
				if (mesh_set->do_next_step() < 0)
					return -1;
		
			for (int j = 0; j < sw.size(); j++)
				sw[j]->dump(i);

			if( (cur_time = mesh_set->get_current_time()) < 0)
				return -1;
			*logger << "Finished step " << i << ". Time = " << cur_time < ".";
		}

		// TODO: delete all objects?
	}
	catch (GCMException& e)
	{
		// print error message
		*logger << "ERROR: " < e.getMessage();
		// terminate all other procs
		// FIXME
		// return -1 works too, but I think it's better to call a specialized
		// function to stop execution 
		data_bus->terminate();
	}

	return 0;
}
