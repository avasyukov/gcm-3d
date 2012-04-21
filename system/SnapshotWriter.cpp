#include "SnapshotWriter.h"

SnapshotWriter::SnapshotWriter()
{
	snapshot_writer_type.assign("Generic snapshot writer");
};

SnapshotWriter::SnapshotWriter(string new_result_dir)
{
	SnapshotWriter();
	set_result_dir(new_result_dir);
};

void SnapshotWriter::set_result_dir(string new_result_dir)
{
	result_dir = new_result_dir;
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
	if(tetr_mesh == NULL)
		throw GCMException( GCMException::SNAP_EXCEPTION, "No mesh provided");

	*logger < "WARN: SnapshotWriter::dump_tetr_mesh - not yet implemented!";	

	return 0;
};

int SnapshotWriter::dump_vtk(TetrMeshSet* mesh_set, int snap_num)
{
	for(int i = 0; i < mesh_set->get_number_of_local_meshes(); i++) 
		if (mesh_set->get_local_mesh(i)->local)
		{
			if( dump_vtk( mesh_set->get_local_mesh(i), snap_num ) < 0 )
				return -1;
		}
	return 0;
};

// TODO - think about local, remote, unused, etc
int SnapshotWriter::dump_vtk(TetrMesh_1stOrder* tetr_mesh, int snap_num)
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

	stringstream name;
	name << result_dir << "snap_volume_zone_" << zone_num << "_snap_" << snap_num << ".vtu";
	string filename = name.str();

	xgw->SetInput(g);
	xgw->SetFileName(filename.c_str());
	xgw->Update();

	xgw->Delete();
	g->Delete();
	pts->Delete();
	tetra->Delete();

	return 0;
};

