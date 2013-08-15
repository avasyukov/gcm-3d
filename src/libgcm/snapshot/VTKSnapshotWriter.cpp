#include "VTKSnapshotWriter.h"

gcm::VTKSnapshotWriter::VTKSnapshotWriter() {
	INIT_LOGGER("gcm.VTKSnapshotWriter");
	fname = string ("snap_mesh_%m_cpu_%z_step_%n.vtu");
}

gcm::VTKSnapshotWriter::VTKSnapshotWriter(const char* snapName) {
	INIT_LOGGER("gcm.VTKSnapshotWriter");
	fname = string (snapName);
}

gcm::VTKSnapshotWriter::~VTKSnapshotWriter() {
	
}

void gcm::VTKSnapshotWriter::init() {
	
}

string gcm::VTKSnapshotWriter::getType() {
	return "VTKSnapshotWriter";
}

void gcm::VTKSnapshotWriter::dump(TetrMeshFirstOrder* mesh, int step)
{
	dump((TetrMeshSecondOrder*)mesh, step);
}

void gcm::VTKSnapshotWriter::dump(TetrMeshSecondOrder* mesh, int step)
{
	dumpVTK(getFileName(MPI::COMM_WORLD.Get_rank(), step, mesh->getId()), mesh, step);
}

string gcm::VTKSnapshotWriter::getFileName(int cpuNum, int step, string meshId) {
	string filename = fname;
	replaceAll (filename, "%z", t_to_string (cpuNum));
	replaceAll (filename, "%n", t_to_string (step));
	replaceAll (filename, "%m", meshId);
	return filename;
}

void gcm::VTKSnapshotWriter::setFileName(string name) {
	fname = name;
}

void gcm::VTKSnapshotWriter::dumpVTK(string filename, TetrMeshSecondOrder *mesh, int step)
{
	map<int, int> snapNodeMap;
	
	vtkXMLUnstructuredGridWriter *xgw = vtkXMLUnstructuredGridWriter::New();
	vtkUnstructuredGrid *g = vtkUnstructuredGrid::New();

	CalcNode* node;
	TetrSecondOrder* tetr;

	vtkPoints *pts = vtkPoints::New();

	vtkDoubleArray *vel = vtkDoubleArray::New();
	vel->SetNumberOfComponents(3);
	vel->SetName("velocity");
	vtkIntArray	   *nodeNumber = vtkIntArray::New ();
	vtkDoubleArray *contact = vtkDoubleArray::New();
	vtkIntArray	   *nodePublicFlags = vtkIntArray::New ();
	vtkIntArray	   *nodePrivateFlags = vtkIntArray::New ();
	vtkIntArray	   *nodeErrorFlags = vtkIntArray::New ();
	vtkDoubleArray *sxx = vtkDoubleArray::New();
	vtkDoubleArray *sxy = vtkDoubleArray::New();
	vtkDoubleArray *sxz = vtkDoubleArray::New();
	vtkDoubleArray *syy = vtkDoubleArray::New();
	vtkDoubleArray *syz = vtkDoubleArray::New();
	vtkDoubleArray *szz = vtkDoubleArray::New();
	vtkIntArray    *matId = vtkIntArray::New();
	vtkDoubleArray *rho = vtkDoubleArray::New();

	float v[3];
	int snapNodeCount = 0;
	
	for(int i = 0; i < mesh->getNodesNumber(); i++) {
	//for( MapIter itr = mesh->nodesMap.begin(); itr != mesh->nodesMap.end(); ++itr ) {
		node = mesh->getNodeByLocalIndex(i);
		
		if( node->isFirstOrder() )
		{
			snapNodeMap[ node->number ] = snapNodeCount;
			snapNodeCount++;
			pts->InsertNextPoint( node->coords[0], node->coords[1], node->coords[2] );
			v[0] = node->values[0];	v[1] = node->values[1];	v[2] = node->values[2];
			vel->InsertNextTuple(v);
			sxx->InsertNextValue( node->values[3] );
			sxy->InsertNextValue( node->values[4] );
			sxz->InsertNextValue( node->values[5] );
			syy->InsertNextValue( node->values[6] );
			syz->InsertNextValue( node->values[7] );
			szz->InsertNextValue( node->values[8] );
			matId->InsertNextValue( node->getMaterialId() );
			rho->InsertNextValue( node->getRho() );
			nodeNumber->InsertNextValue( node->number );
			contact->InsertNextValue( node->isInContact () ? 1 : 0 );
			nodePublicFlags->InsertNextValue (node->getPublicFlags ());
			nodePrivateFlags->InsertNextValue (node->getPrivateFlags ());
			nodeErrorFlags->InsertNextValue (node->getErrorFlags());
		}
	}
	g->SetPoints(pts);

	vtkIntArray* tetr1stOrderNodes = vtkIntArray::New ();
	tetr1stOrderNodes->SetNumberOfComponents (4);
	vtkIntArray* tetrNumber = vtkIntArray::New ();
	vtkTetra *tetra=vtkTetra::New();
	for(int i = 0; i < mesh->getTetrsNumber(); i++) {
		/*tetr = mesh->getTetr(i);
		tetra->GetPointIds()->SetId(0,tetr->verts[0]);
		tetra->GetPointIds()->SetId(1,tetr->verts[1]);
		tetra->GetPointIds()->SetId(2,tetr->verts[2]);
		tetra->GetPointIds()->SetId(3,tetr->verts[3]);*/
		//tetr = &(mesh->tetrs1[i]);
		tetr = mesh->getTetr2ByLocalIndex(i);
		for( int z = 0; z < 4; z++)
		{
			int snapIndex = snapNodeMap[ tetr->verts[z] ];
			tetra->GetPointIds()->SetId( z, snapIndex );
		}
		g->InsertNextCell(tetra->GetCellType(),tetra->GetPointIds());
		
		tetr1stOrderNodes->InsertNextTupleValue (tetr->verts);
		tetrNumber->InsertNextValue (tetr->number);
		
		/*if( MPI::COMM_WORLD.Get_rank() == 1)
		{
			LOG_DEBUG("Tetr A: " << tetr->verts[0] 
								<< " " << tetr->verts[1] 
								<< " " << tetr->verts[2] 
								<< " " << tetr->verts[3] );
			LOG_DEBUG("Tetr B: " << mesh->nodesMap[tetr->verts[0]] 
								<< " " << mesh->nodesMap[tetr->verts[1]] 
								<< " " << mesh->nodesMap[tetr->verts[2]] 
								<< " " << mesh->nodesMap[tetr->verts[3]] );
		}*/
	}

	sxx->SetName("sxx");
	sxy->SetName("sxy");
	sxz->SetName("sxz");
	syy->SetName("syy");
	syz->SetName("syz");
	szz->SetName("szz");
	matId->SetName("materialID");
	rho->SetName("rho");
	contact->SetName("contact");
	nodeNumber->SetName("nodeNumber");
	nodePublicFlags->SetName ("publicFlags");
	nodePrivateFlags->SetName ("privateFlags");
	nodeErrorFlags->SetName ("errorFlags");
	
	g->GetPointData()->SetVectors(vel);
	g->GetPointData()->AddArray(sxx);
	g->GetPointData()->AddArray(sxy);
	g->GetPointData()->AddArray(sxz);
	g->GetPointData()->AddArray(syy);
	g->GetPointData()->AddArray(syz);
	g->GetPointData()->AddArray(szz);
	g->GetPointData()->AddArray(matId);
	g->GetPointData()->AddArray(rho);
	g->GetPointData()->AddArray(nodeNumber);
	g->GetPointData()->AddArray(contact);
	g->GetPointData ()->AddArray (nodePublicFlags);
	g->GetPointData ()->AddArray (nodePrivateFlags);
	g->GetPointData ()->AddArray (nodeErrorFlags);
	
	tetr1stOrderNodes->SetName ("tetr1stOrderNodes");
	g->GetCellData ()->AddArray (tetr1stOrderNodes);
	tetrNumber->SetName ("tetrNumber");
	g->GetCellData ()->AddArray (tetrNumber);
	
	vel->Delete();
	sxx->Delete();
	sxy->Delete();
	sxz->Delete();
	syy->Delete();
	syz->Delete();
	szz->Delete();
	matId->Delete();
	rho->Delete();
	nodeNumber->Delete();
	contact->Delete();
	nodePublicFlags->Delete ();
	nodePrivateFlags->Delete ();
	nodeErrorFlags->Delete ();
	
	tetr1stOrderNodes->Delete ();
	tetrNumber->Delete ();
	
	xgw->SetInput(g);
	xgw->SetFileName(filename.c_str());
	xgw->Update();
	
	xgw->Delete();
	g->Delete();
	pts->Delete();
	tetra->Delete();
}