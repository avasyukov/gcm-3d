#include "LineSnapshotWriter.h"

int line_compar(const void *p1, const void *p2)
{
	return ((float*)p1)[0] > ((float*)p2)[0];
}

LineSnapshotWriter::LineSnapshotWriter(char *param)
{
	from[0] = from[1] = from[2] = to[0] = to[1] = to[2] = dir[0] = dir[1] = 0.0;
	dir[2] = 1.0;
	len = 6.0;
	threshold = 0.1;
	resultdir = "./";
	if (!strcmp("@", param))
		fname = "snap_line_%n.txt";
	else
		fname = param;	
}

void LineSnapshotWriter::dump(int snap_num)
{
	DataBus *data_bus = DataBus::getInstance();
	TetrMeshSet *mesh_set = TetrMeshSet::getInstance();
	vector<float> line;

	MPI::COMM_WORLD.Barrier();	
	*logger < "Building line dump";
	for (int i = 0; i < mesh_set->get_number_of_local_meshes(); i++)
	{
		TetrMesh_1stOrder *mesh = mesh_set->get_local_mesh(i);
		for (int q = 0; q < mesh->tetrs.size(); q++)
		{
			Tetrahedron_1st_order *tetr = &mesh->tetrs[0]+q;
			for (int j = 0; j < 4; j++)
			{
				int i1 = (0+j) % 4;
				int i2 = (1+j) % 4;
				int i3 = (2+j) % 4;
				float *c1 = mesh->nodes[tetr->vert[i1]].coords;
				float *c2 = mesh->nodes[tetr->vert[i2]].coords;
				float *c3 = mesh->nodes[tetr->vert[i3]].coords;
				float c[3];
				if (mesh->vector_intersects_triangle(c1, c2, c3, from, dir, len, c))
				{
					if (!(mesh->nodes[tetr->vert[i1]].placement_type == LOCAL && 
							mesh->nodes[tetr->vert[i2]].placement_type == LOCAL && 
							mesh->nodes[tetr->vert[i3]].placement_type == LOCAL))
						continue;
					bool flag = false;
					float p = 0;
					for (int k = 0; k < 3; k++)
						p += (c[k]-fromr[k])*(c[k]-fromr[k]);
					p = sqrt(p);					
					for (int k = 0; k < line.size(); k+=14)
						if (fabs(line[k]-p) < threshold)
						{
							flag = true;
							break;
						}
					if (flag)
						continue;
					float *v1 = mesh->nodes[tetr->vert[i1]].values;
					float *v2 = mesh->nodes[tetr->vert[i2]].values;
					float *v3 = mesh->nodes[tetr->vert[i3]].values;
					float v[13];
					mesh->interpolate_triangle(c1, c2, c3, c, v1, v2, v3, v, 13);
					line.push_back(p);
					for (int k = 0; k < 13; k++)
						line.push_back(v[k]);
					break;
				}
			}
		}
	}
	
	int sz = line.size();
	int *cnts;
	if (!data_bus->get_proc_num())
		cnts = new int[data_bus->get_procs_total_num()];
	MPI::COMM_WORLD.Gather(&sz, 1, MPI::INT, cnts, 1, MPI::INT, 0);
	
	int tsz = 0; 
	if (!data_bus->get_proc_num())
		for (int i = 0; i < data_bus->get_procs_total_num(); i++)
			tsz += cnts[i];
	float *vals;
	int *displs;
	if (!data_bus->get_proc_num())
	{
		vals = new float[tsz];
		displs = new int[data_bus->get_procs_total_num()];
		displs[0] = 0;
		for (int i = 0; i < data_bus->get_procs_total_num()-1; i++)
			displs[i+1] = displs[i]+cnts[i];
	}

	MPI::COMM_WORLD.Gatherv(&line[0], sz, MPI::FLOAT, vals, cnts, displs, MPI::FLOAT, 0);
	
	if (!data_bus->get_proc_num())
	{
		qsort(vals, tsz/14, sizeof(float)*14, &line_compar);
		string filename = fname;
		Utils::replaceAll(filename, "%n", Utils::t_to_string(snap_num));
		filename = resultdir+filename;
		*logger << "Dumping line snapshot to file " < filename;
		ofstream *outs = new ofstream(filename.c_str(), fstream::out);	
		outs->setf(ios::fixed,ios::floatfield);
		outs->	precision(10);
		for (int i = 0; i < tsz; i+= 14)
		{
			for (int j = 0; j < 14; j++)
				*outs << vals[i+j] << '\t';
			*outs << endl;
		}
		outs->close();
						
		delete outs;
		delete[] vals;
		delete[] displs;
		delete[] cnts;
	}
	
	MPI::COMM_WORLD.Barrier();
}

void LineSnapshotWriter::parseArgs(int argc, char **argv)
{
	static struct option long_options[] =
	{
		{"from"      , required_argument, 0, 'F'},
		{"to"        , required_argument, 0, 'T'},
		{"output-dir", required_argument, 0, 'o'},		
		{"threshold" , required_argument, 0, 'H'},
		{0     , 0                , 0, 0  }
	};
	
	int option_index = 0;

	int c;
	while ((c = getopt_long (argc, argv, "F:T:o:", long_options, &option_index)) != -1)
	{
		*logger << "Parsing argument: " < (char)c;
		switch (c)
		{
			case 'o':
				resultdir = optarg;
				if (resultdir[resultdir.length()-1] != '/')
					resultdir += '/';
				break;
			case 'F':
				if (sscanf(optarg, "%f,%f,%f", from, from+1, from+2) != 3)
					throw new GCMException(GCMException::CONFIG_EXCEPTION);
				for (int i = 0; i < 3; i++)
					fromr[i] = from[i];
				break;
			case 'T':
				if (sscanf(optarg, "%f,%f,%f", to, to+1, to+2) != 3)
					throw new GCMException(GCMException::CONFIG_EXCEPTION);
				break;
			case 'H':
				threshold = atof(optarg);
				break;
			default:
				optind--;
				return;
		}
	}
}

void LineSnapshotWriter::init()
{
	len = 0.0;	
		
	for (int i = 0; i < 3; i++)
	{
		dir[i] = to[i]-from[i];
		len += dir[i]*dir[i];
	}
	
	len = sqrt(len)+5;
	
	for (int i = 0; i < 3; i++)
		dir[i] /= len;
	
	for (int i = 0; i < 3; i++)
	{
		from[i] -= dir[i];
		to[i] += dir[i];
	}		
}
