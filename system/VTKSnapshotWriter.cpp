#include "VTKSnapshotWriter.h"

VTKSnapshotWriter::VTKSnapshotWriter(char *param)
{
	snapshot_writer_type.assign("Generic snapshot writer");
	resultdir = "./";
	if (!strcmp("@", param))
		fname = "snap_volume_zone_%z_snap_%n.vtu";
	else
		fname = param;
};

string* VTKSnapshotWriter::get_snapshot_writer_type()
{
	return &snapshot_writer_type;
};

int VTKSnapshotWriter::dump_tetr_mesh(TetrMesh_1stOrder* tetr_mesh, int zone_num, int snap_num)
{
	if(tetr_mesh == NULL)
		throw GCMException( GCMException::SNAP_EXCEPTION, "No mesh provided");

	*logger < "WARN: SnapshotWriter::dump_tetr_mesh - not yet implemented!";	

	return 0;
};

int VTKSnapshotWriter::dump_vtk(int snap_num)
{
	TetrMeshSet *mesh_set = TetrMeshSet::getInstance();
	for(int i = 0; i < mesh_set->get_number_of_local_meshes(); i++) 
		if( dump_vtk( mesh_set->get_local_mesh(i), snap_num ) < 0 )
			return -1;
	return 0;
};

// TODO - think about local, remote, unused, etc
int VTKSnapshotWriter::dump_vtk(TetrMesh_1stOrder* tetr_mesh, int snap_num)
{
	int zone_num = tetr_mesh->zone_num;

	vtkXMLUnstructuredGridWriter *xgw = vtkXMLUnstructuredGridWriter::New();
	vtkUnstructuredGrid *g = vtkUnstructuredGrid::New();

	ElasticNode node;
	Tetrahedron_1st_order tetr;

	vtkPoints *pts = vtkPoints::New();

	vtkDoubleArray *vel = vtkDoubleArray::New();
	vel->SetNumberOfComponents(3);
	vel->SetName("velocity");
	vtkDoubleArray *contact = vtkDoubleArray::New();
	vtkDoubleArray *sxx = vtkDoubleArray::New();
	vtkDoubleArray *sxy = vtkDoubleArray::New();
	vtkDoubleArray *sxz = vtkDoubleArray::New();
	vtkDoubleArray *syy = vtkDoubleArray::New();
	vtkDoubleArray *syz = vtkDoubleArray::New();
	vtkDoubleArray *szz = vtkDoubleArray::New();
	vtkDoubleArray *la = vtkDoubleArray::New();
	vtkDoubleArray *mu = vtkDoubleArray::New();
	vtkDoubleArray *rho = vtkDoubleArray::New();

	float v[3];

	for(int i = 0; i < (tetr_mesh->nodes).size(); i++) {
		node = (tetr_mesh->nodes)[i];
		pts->InsertNextPoint( node.coords[0], node.coords[1], node.coords[2] );
		v[0] = node.values[0];	v[1] = node.values[1];	v[2] = node.values[2];
		vel->InsertNextTuple(v);
		sxx->InsertNextValue( node.values[3] );
		sxy->InsertNextValue( node.values[4] );
		sxz->InsertNextValue( node.values[5] );
		syy->InsertNextValue( node.values[6] );
		syz->InsertNextValue( node.values[7] );
		szz->InsertNextValue( node.values[8] );
		la->InsertNextValue( node.la );
		mu->InsertNextValue( node.mu );
		rho->InsertNextValue( node.rho );
		contact->InsertNextValue( node.contact_type );
	}
	g->SetPoints(pts);

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
	la->SetName("lambda");
	mu->SetName("mu");
	rho->SetName("rho");
	contact->SetName("contact");

	g->GetPointData()->SetVectors(vel);
	g->GetPointData()->AddArray(sxx);
	g->GetPointData()->AddArray(sxy);
	g->GetPointData()->AddArray(sxz);
	g->GetPointData()->AddArray(syy);
	g->GetPointData()->AddArray(syz);
	g->GetPointData()->AddArray(szz);
	g->GetPointData()->AddArray(la);
	g->GetPointData()->AddArray(mu);
	g->GetPointData()->AddArray(rho);
	g->GetPointData()->AddArray(contact);

	vel->Delete();
	sxx->Delete();
	sxy->Delete();
	sxz->Delete();
	syy->Delete();
	syz->Delete();
	szz->Delete();
	la->Delete();
	mu->Delete();
	rho->Delete();
	contact->Delete();

	string filename = fname;
	Utils::replaceAll(filename, "%z", Utils::t_to_string(zone_num));
	Utils::replaceAll(filename, "%n", Utils::t_to_string(snap_num));
	filename = resultdir+filename;
	
	*logger << "Dumping VTK snapshot to file " < filename;

	xgw->SetInput(g);
	xgw->SetFileName(filename.c_str());
	xgw->Update();

	xgw->Delete();
	g->Delete();
	pts->Delete();
	tetra->Delete();

	return 0;
};

void VTKSnapshotWriter::dump(int snap_num)
{
	dump_vtk(snap_num);
}

void VTKSnapshotWriter::parseArgs(int argc, char **argv)
{
	static struct option long_options[] =
	{
		{"output-dir", required_argument, 0, 'o'},
		{0           , 0                , 0, 0  }
	};
	
	int option_index = 0;

	int c;
	while ((c = getopt_long (argc, argv, "o:", long_options, &option_index)) != -1)
		switch (c)
		{
			case 'o':
				resultdir = optarg;
				if (resultdir[resultdir.length()-1] != '/')
					resultdir += '/';
				break;
			default:
				optind--;
				return;
		}
}

void VTKSnapshotWriter::init()
{
}