SnapshotWriter::SnapshotWriter()
{
	snapshot_writer_type.assign("Generic snapshot writer");
};

SnapshotWriter::~SnapshotWriter() { };

string* SnapshotWriter::get_snapshot_writer_type()
{
	return &snapshot_writer_type;
};

void SnapshotWriter::attach(Logger* new_logger)
{
	logger = new_logger;
};

int SnapshotWriter::dump_tetr_mesh(TetrMesh_1stOrder* tetr_mesh, string file_name)
{
	if(logger != NULL)
		logger->write(string("ERROR: SnapshotWriter::dump_tetr_mesh - not implemented yet!"));
	return -1;
};


void SnapshotWriter::set_basement(float x0, float y0, float z0, float h, int N)
{
	base_coord[0] = x0;
	base_coord[1] = y0;
	base_coord[2] = z0;
	step_h = h;
	number_of_segments = N;
};

int SnapshotWriter::dump_cubic_mesh(TetrMesh_1stOrder* tetr_mesh, string file_name)
{
	ElasticNode nodes[number_of_segments+1][number_of_segments+1][number_of_segments+1];
	int node_index_min[3];
	int node_index_max[3];
	float min_coord[3];
	float max_coord[3];

	if(tetr_mesh == NULL) {
		if(logger != NULL)
			logger->write(string("ERROR: SnapshotWriter::dump_cubic_mesh - mesh is NULL!"));
		return -1;
	}

	if(step_h < tetr_mesh->get_min_h() * 5) // TODO it is slow... and avoid magick number
	{
		if(logger != NULL)
			logger->write(string("ERROR: SnapshotWriter::dump_cubic_mesh - h for dump is too small!"));
		return -1;
	}

	// Clear nodes of cubic mesh
	for(int i = 0; i < number_of_segments+1; ++i)
		for(int j = 0; j < number_of_segments+1; ++j)
			for(int k = 0; k < number_of_segments+1; ++k)
			{
				zero_node_values(&nodes[i][j][k]);
				nodes[i][j][k].coords[0] = nodes[i][j][k].fixed_coords[0] = base_coord[0] + i*step_h;
				nodes[i][j][k].coords[1] = nodes[i][j][k].fixed_coords[1] = base_coord[1] + j*step_h;
				nodes[i][j][k].coords[2] = nodes[i][j][k].fixed_coords[2] = base_coord[2] + k*step_h;
			}

	// Go through tetrahedrons
	for(int i = 0; i < (tetr_mesh->tetrs).size(); i++)
	{
		// Just remember verticles numbers to simplify the code further
		int v[4];
		for(int j = 0; j < 4; j++)
			v[j] = (tetr_mesh->tetrs).at(i).vert[j];

		// Find minimum and maximum values for all coordinates
		for(int j = 0; j < 3; j++)
		{
			min_coord[j] = min_of_four( (tetr_mesh->nodes).at(v[0]).coords[j], (tetr_mesh->nodes).at(v[1]).coords[j],
							(tetr_mesh->nodes).at(v[2]).coords[j], (tetr_mesh->nodes).at(v[3]).coords[j] );
			max_coord[j] = max_of_four( (tetr_mesh->nodes).at(v[0]).coords[j], (tetr_mesh->nodes).at(v[1]).coords[j],
							(tetr_mesh->nodes).at(v[2]).coords[j], (tetr_mesh->nodes).at(v[3]).coords[j] );
		}

		bool may_be_found = true;

		// Find indexes of nodes of cubic mesh on 'borders' of virtual cube given as minimum and maximum coords
		for(int j = 0; j < 3; j++)
		{
			node_index_min[j] = (int)( (min_coord[j] - base_coord[j]) / step_h );
				if( (min_coord[j] - base_coord[j]) < 0 ) { node_index_min[j] = -1; } // workaround to fix (int)(-0.1) == 0.0
			node_index_max[j] = (int)( (max_coord[j] - base_coord[j]) / step_h );

			// If indexes are equal or out of range - the point can not be inside even this bigger cube
			if( ((node_index_max[j]-node_index_min[j]) != 1) || (node_index_max[j]<0) || (node_index_min[j]>number_of_segments) )
				may_be_found = false;
		}

		// If we suspect that verticle can be in this tetrahedron
		if(may_be_found)
		{
			// Check it to be sure
			if( tetr_mesh->point_in_tetr( nodes[node_index_max[0]][node_index_max[1]][node_index_max[2]].coords[0],
							nodes[node_index_max[0]][node_index_max[1]][node_index_max[2]].coords[1],
							nodes[node_index_max[0]][node_index_max[1]][node_index_max[2]].coords[2],
							&((tetr_mesh->tetrs).at(i)) ) )
			{
				// And interpolate node in question
				tetr_mesh->interpolate( &(nodes[node_index_max[0]][node_index_max[1]][node_index_max[2]]),
							&((tetr_mesh->tetrs).at(i)) );
			}
		}

		// Write to disk
	}

	return 0;
};

void SnapshotWriter::zero_node_values(ElasticNode* node)
{
	for(int l = 0; l < 8; l++)
		node->values[l] = 0;
	node->la = 0;
	node->mu = 0;
	node->rho = 0;	
};

float SnapshotWriter::min_of_four(float a, float b, float c, float d)
{
	float res = a;
	if(b < res)
		res = b;
	if(c < res)
		res = c;
	if(d < res)
		res = d;
	return res;
};

float SnapshotWriter::max_of_four(float a, float b, float c, float d)
{
	float res = a;
	if(b > res)
		res = b;
	if(c > res)
		res = c;
	if(d > res)
		res = d;
	return res;
};
