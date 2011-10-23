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

void SnapshotWriter::set_basement(float x0, float y0, float z0, float h, int N)
{
	base_coord[0] = x0;
	base_coord[1] = y0;
	base_coord[2] = z0;
	step_h = h;
	number_of_segments = N;
};

int SnapshotWriter::dump_tetr_mesh(TetrMesh_1stOrder* tetr_mesh, int zone_num, int snap_num)
{
	if(tetr_mesh == NULL) {
		if(logger != NULL)
			logger->write(string("ERROR: SnapshotWriter::dump_tetr_mesh - mesh is NULL!"));
		return -1;
	}

	if(logger != NULL)
		logger->write(string("WARN: SnapshotWriter::dump_tetr_mesh - not yet implemented!"));	

	return 0;
};

// TODO - think about local, remote, unused, etc
int SnapshotWriter::dump_vtk(TetrMesh_1stOrder* tetr_mesh, int zone_num, int snap_num)
{
	vtkXMLUnstructuredGridWriter *xgw = vtkXMLUnstructuredGridWriter::New();
	vtkUnstructuredGrid *g = vtkUnstructuredGrid::New();

	ElasticNode node;
	Tetrahedron_1st_order tetr;

	vtkPoints *pts = vtkPoints::New();

	vtkDoubleArray *sxx = vtkDoubleArray::New();
	vtkDoubleArray *sxy = vtkDoubleArray::New();
	vtkDoubleArray *sxz = vtkDoubleArray::New();
	vtkDoubleArray *syy = vtkDoubleArray::New();
	vtkDoubleArray *syz = vtkDoubleArray::New();
	vtkDoubleArray *szz = vtkDoubleArray::New();
	vtkDoubleArray *vx = vtkDoubleArray::New();
	vtkDoubleArray *vy = vtkDoubleArray::New();
	vtkDoubleArray *vz = vtkDoubleArray::New();

	vtkDoubleArray *norm = vtkDoubleArray::New();
	norm->SetNumberOfComponents(3);
	norm->SetName("normal");

	float v[3];

	for(int i = 0; i < (tetr_mesh->nodes).size(); i++) {
		node = (tetr_mesh->nodes)[i];
		pts->InsertNextPoint( node.coords[0], node.coords[1], node.coords[2] );
		vx->InsertNextValue( node.values[0] );
		vy->InsertNextValue( node.values[1] );
		vz->InsertNextValue( node.values[2] );
		sxx->InsertNextValue( node.values[3] );
		sxy->InsertNextValue( node.values[4] );
		sxz->InsertNextValue( node.values[5] );
		syy->InsertNextValue( node.values[6] );
		syz->InsertNextValue( node.values[7] );
		szz->InsertNextValue( node.values[8] );

		if(node.border_type == BORDER) {
			tetr_mesh->find_border_node_normal(i, &v[0], &v[1], &v[2]);
		} else {
			v[0] = 0;	v[1] = 0;	v[2] = 0;
		}
		norm->InsertNextTuple(v);
	}
	g->SetPoints(pts);

	g->GetPointData()->SetVectors(norm);

	vtkTetra *tetra=vtkTetra::New();
	for(int i = 0; i < (tetr_mesh->tetrs).size(); i++) {
		tetr = (tetr_mesh->tetrs)[i];
		tetra->GetPointIds()->SetId(0,tetr.vert[0]);
		tetra->GetPointIds()->SetId(1,tetr.vert[1]);
		tetra->GetPointIds()->SetId(2,tetr.vert[2]);
		tetra->GetPointIds()->SetId(3,tetr.vert[3]);
		g->InsertNextCell(tetra->GetCellType(),tetra->GetPointIds());
	}

	sxx->SetName("sxx");
	sxy->SetName("sxy");
	sxz->SetName("sxz");
	syy->SetName("syy");
	syz->SetName("syz");
	szz->SetName("szz");
	vx->SetName("vx");
	vy->SetName("vy");
	vz->SetName("vz");

	g->GetPointData()->AddArray(sxx);
	g->GetPointData()->AddArray(sxy);
	g->GetPointData()->AddArray(sxz);
	g->GetPointData()->AddArray(syy);
	g->GetPointData()->AddArray(syz);
	g->GetPointData()->AddArray(szz);
	g->GetPointData()->AddArray(vx);
	g->GetPointData()->AddArray(vy);
	g->GetPointData()->AddArray(vz);

	sxx->Delete();
	sxy->Delete();
	sxz->Delete();
	syy->Delete();
	syz->Delete();
	szz->Delete();
	vx->Delete();
	vy->Delete();
	vz->Delete();

	stringstream name;
	name << "snap_volume_" << snap_num << ".vtu";
	string filename = name.str();

	xgw->SetInput(g);
	xgw->SetFileName(filename.c_str());
	xgw->Update();

	xgw->Delete();
	g->Delete();
	pts->Delete();
	tetra->Delete();
	norm->Delete();

	return 0;
};

int SnapshotWriter::tmp_dump_line(TetrMesh_1stOrder* tetr_mesh, int snap_num)
{
	ofstream dumpfile;

//	cout << "DEBUG: h=" << step_h << " max_h=" << tetr_mesh->get_max_h() << " min_h=" << tetr_mesh->get_min_h() << endl;

	// Write to disk
	stringstream ss;
	stringstream name;

	// Separate dump for each scalar
	for(int l = 0; l < 9; ++l)
	{
		ss.str("");
		name.str("");
		switch (l) {
			case 0:
				ss << "vx"; break;
			case 1:
				ss << "vy"; break;
			case 2:
				ss << "vz"; break;
			case 3:
				ss << "sxx"; break;
			case 4:
				ss << "sxy"; break;
			case 5:
				ss << "sxz"; break;
			case 6:
				ss << "syy"; break;
			case 7:
				ss << "syz"; break;
			case 8:
				ss << "szz"; break;
		}

		name << "line_snap_" << snap_num << "_" << ss.str() << ".vtk";
		string filename = name.str();
	  	dumpfile.open(filename.c_str());
		//cout << "DEBUG: " << filename.c_str() << endl;
		if(!dumpfile.is_open())
		{
			return -1;
		}

		for(int i = 0; i < (tetr_mesh->nodes).size(); i++)
		{
			if( ( fabs( (tetr_mesh->nodes).at(i).coords[1] - 1.75 ) < 0.15 ) 
				&& ( fabs( (tetr_mesh->nodes).at(i).coords[2] - 1.75 ) < 0.15 ) )
			{
				dumpfile << (tetr_mesh->nodes).at(i).coords[0] << " " << (tetr_mesh->nodes).at(i).values[l] << endl;
			}
		}
		dumpfile.close();
	}

	return 0;
};

/*int SnapshotWriter::dump_cubic_mesh(TetrMesh_1stOrder* tetr_mesh, int zone_num, int snap_num)
{
	ofstream dumpfile;

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

	cout << "DEBUG: h=" << step_h << " max_h=" << tetr_mesh->get_max_h() << " min_h=" << tetr_mesh->get_min_h() << endl;

	if(step_h < tetr_mesh->get_max_h() * 2) // TODO it is slow... and avoid magick number
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
			min_coord[j] -= 0.01 * fabs(min_coord[j]); // workaround to prevent issues with round-up
			max_coord[j] += 0.01 * fabs(max_coord[j]); // virtual cube will be 1% bigger each dimension
			node_index_min[j] = (int)( (min_coord[j] - base_coord[j]) / step_h );
				if( (min_coord[j] - base_coord[j]) < 0 ) { node_index_min[j] = -1; } // workaround to fix (int)(-0.1) == 0.0
			node_index_max[j] = (int)( (max_coord[j] - base_coord[j]) / step_h );

			// If indexes are equal or out of range - the point can not be inside even this bigger cube
			if( ((node_index_max[j]-node_index_min[j]) != 1) || (node_index_max[j]<0) || (node_index_min[j]>number_of_segments) )
//			{
				may_be_found = false;
//			} else {
//                                cout << "DEBUG: " << min_coord[j] << " " << base_coord[j] << " " << max_coord[j] << " " << node_index_min[j] << " " << node_index_max[j] << endl;
//			}
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
				if (tetr_mesh->interpolate(
					&(nodes[node_index_max[0]][node_index_max[1]][node_index_max[2]]),
					&((tetr_mesh->tetrs).at(i)) ) < 0)
				{
					if(logger != NULL)
						logger->write(string("ERROR: SnapshotWriter::dump_cubic_mesh - interpolation failed!"));
					return -1;
				}
			}
		}
	}

	// Write to disk
	stringstream ss;
	stringstream name;

	// Separate dump for each scalar
	for(int l = 0; l < 9; ++l)
	{
		ss.str("");
		name.str("");
		switch (l) {
			case 0:
				ss << "vx"; break;
			case 1:
				ss << "vy"; break;
			case 2:
				ss << "vz"; break;
			case 3:
				ss << "sxx"; break;
			case 4:
				ss << "sxy"; break;
			case 5:
				ss << "sxz"; break;
			case 6:
				ss << "syy"; break;
			case 7:
				ss << "syz"; break;
			case 8:
				ss << "szz"; break;
		}

		name << "snap_" << snap_num << "_zone_" << zone_num << "_" << ss.str() << ".vtk";
		string filename = name.str();
	  	dumpfile.open(filename.c_str());
		//cout << "DEBUG: " << filename.c_str() << endl;
		if(!dumpfile.is_open())
		{
			if(logger != NULL)
				logger->write(string("ERROR: SnapshotWriter::dump_cubic_mesh - can not open file for snapshot!"));
			return -1;
		}

		dumpfile << "# vtk DataFile Version 2.0" << endl;
		dumpfile << "Volume " << "snap_" << snap_num << "_zone_" << zone_num << endl;
		dumpfile << "ASCII" << endl;
		dumpfile << "DATASET STRUCTURED_POINTS" << endl;
		dumpfile << "DIMENSIONS " << number_of_segments+1 << " " << number_of_segments+1 << " " << number_of_segments+1 << endl;
		dumpfile << "SPACING " << step_h << " " << step_h << " " << step_h << endl;
		dumpfile << "ORIGIN " << base_coord[0] << " " << base_coord[1] << " " << base_coord[2] << endl;
		dumpfile << "POINT_DATA " << (number_of_segments+1) * (number_of_segments+1) * (number_of_segments+1) << endl;
		dumpfile << "SCALARS " << ss.str() << " float 1" << endl;
		dumpfile << "LOOKUP_TABLE default" << endl;

		for(int k = 0; k < number_of_segments+1; ++k)
		{
			for(int j = 0; j < number_of_segments+1; ++j)
			{
				for(int i = 0; i < number_of_segments+1; ++i)	
				{
					dumpfile << nodes[i][j][k].values[l] << " ";
				}
			}
			// dumpfile << endl;
		}
		dumpfile.close();
	}

	return 0;
};*/

void SnapshotWriter::zero_node_values(ElasticNode* node)
{
	for(int l = 0; l < 9; l++)
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
