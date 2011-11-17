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
	name << "snap_volume_zone_" << zone_num << "_snap_" << snap_num << ".vtu";
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

