#include "snapshot/VTK2SnapshotWriter.h"

gcm::VTK2SnapshotWriter::VTK2SnapshotWriter() {
	INIT_LOGGER("gcm.VTK2SnapshotWriter");
	fname = string ("dump_mesh_%m_cpu_%z_step_%n.vtu");
}

gcm::VTK2SnapshotWriter::VTK2SnapshotWriter(const char* snapName) {
	INIT_LOGGER("gcm.VTK2SnapshotWriter");
	fname = string (snapName);
}

gcm::VTK2SnapshotWriter::~VTK2SnapshotWriter() {
	
}

string gcm::VTK2SnapshotWriter::getType() {
	return "VTK2SnapshotWriter";
}

void gcm::VTK2SnapshotWriter::dump(Mesh* mesh, int step)
{
	// TODO - check if the mesh is compatible
	dumpVTK(getFileName(MPI::COMM_WORLD.Get_rank(), step, mesh->getId()), (TetrMeshSecondOrder*)mesh, step);
}

void gcm::VTK2SnapshotWriter::dumpVTK(string filename, TetrMeshSecondOrder *mesh, int step)
{
	map<int, int> snapNodeMap;
	
	vtkXMLUnstructuredGridWriter *xgw = vtkXMLUnstructuredGridWriter::New();
	vtkUnstructuredGrid *g = vtkUnstructuredGrid::New();

	vtkPoints *pts = vtkPoints::New();

	vtkDoubleArray *vel = vtkDoubleArray::New();
	vel->SetNumberOfComponents(3);
	vel->SetName("velocity");
	vtkIntArray	   *nodeNumber = vtkIntArray::New ();
	vtkDoubleArray *contact = vtkDoubleArray::New();
	vtkIntArray	   *nodePublicFlags = vtkIntArray::New ();
	vtkIntArray	   *nodePrivateFlags = vtkIntArray::New ();
	vtkIntArray	   *nodeErrorFlags = vtkIntArray::New ();
	vtkIntArray	   *nodeBorderCondId = vtkIntArray::New ();
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
		CalcNode& node = mesh->getNodeByLocalIndex(i);
		
		if( ! shouldSnapshot(node, mesh) )
			continue;
		
		snapNodeMap[ node.number ] = snapNodeCount;
		snapNodeCount++;

		pts->InsertNextPoint( node.coords[0], node.coords[1], node.coords[2] );
		v[0] = node.values[0];	v[1] = node.values[1];	v[2] = node.values[2];
		vel->InsertNextTuple(v);
		sxx->InsertNextValue( node.values[3] );
		sxy->InsertNextValue( node.values[4] );
		sxz->InsertNextValue( node.values[5] );
		syy->InsertNextValue( node.values[6] );
		syz->InsertNextValue( node.values[7] );
		szz->InsertNextValue( node.values[8] );
		matId->InsertNextValue( node.getMaterialId() );
		rho->InsertNextValue( node.getRho() );
		nodeNumber->InsertNextValue( node.number );
		contact->InsertNextValue( node.isInContact () ? 1 : 0 );
		nodePublicFlags->InsertNextValue (node.getPublicFlags ());
		nodePrivateFlags->InsertNextValue (node.getPrivateFlags ());
		nodeErrorFlags->InsertNextValue (node.getErrorFlags());
		nodeBorderCondId->InsertNextValue(node.getBorderConditionId());
		
	}
	g->SetPoints(pts);

	vtkIntArray* tetr1stOrderNodes = vtkIntArray::New ();
	tetr1stOrderNodes->SetNumberOfComponents (4);
	vtkIntArray* tetr2ndOrderNodes = vtkIntArray::New ();
	tetr2ndOrderNodes->SetNumberOfComponents (6);
	vtkIntArray* tetrNumber = vtkIntArray::New ();
	vtkTetra *tetra=vtkTetra::New();
	for(int i = 0; i < mesh->getTetrsNumber(); i++) {
		/*tetr = mesh->getTetr(i);
		tetra->GetPointIds()->SetId(0,tetr.verts[0]);
		tetra->GetPointIds()->SetId(1,tetr.verts[1]);
		tetra->GetPointIds()->SetId(2,tetr.verts[2]);
		tetra->GetPointIds()->SetId(3,tetr.verts[3]);*/
		//tetr = &(mesh->tetrs1[i]);
		TetrSecondOrder& tetr = mesh->getTetr2ByLocalIndex(i);
		bool shouldSnapshotTetr = true;
		for( int z = 0; z < 4; z++)
		{
			CalcNode& node = mesh->getNode( tetr.verts[z] );
			if( ! shouldSnapshot(node, mesh) )
				shouldSnapshotTetr = false;
		}
		for( int z = 0; z < 6; z++)
		{
			CalcNode& node = mesh->getNode( tetr.addVerts[z] );
			if( ! shouldSnapshot(node, mesh) )
				shouldSnapshotTetr = false;
		}
		if( !shouldSnapshotTetr )
			continue;
		
		for( int z = 0; z < 4; z++)
		{
			int snapIndex = snapNodeMap[ tetr.verts[z] ];
			tetra->GetPointIds()->SetId( z, snapIndex );
		}
		g->InsertNextCell(tetra->GetCellType(),tetra->GetPointIds());
		
		tetr1stOrderNodes->InsertNextTupleValue (tetr.verts);
		tetr2ndOrderNodes->InsertNextTupleValue (tetr.addVerts);
		tetrNumber->InsertNextValue (tetr.number);
		
		/*if( MPI::COMM_WORLD.Get_rank() == 1)
		{
			LOG_DEBUG("Tetr A: " << tetr.verts[0] 
								<< " " << tetr.verts[1] 
								<< " " << tetr.verts[2] 
								<< " " << tetr.verts[3] );
			LOG_DEBUG("Tetr B: " << mesh->nodesMap[tetr.verts[0]] 
								<< " " << mesh->nodesMap[tetr.verts[1]] 
								<< " " << mesh->nodesMap[tetr.verts[2]] 
								<< " " << mesh->nodesMap[tetr.verts[3]] );
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
	nodeBorderCondId->SetName("borderCalcId");
	
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
	g->GetPointData()->AddArray(nodeBorderCondId);
	
	tetr1stOrderNodes->SetName ("tetr1stOrderNodes");
	g->GetCellData ()->AddArray (tetr1stOrderNodes);
	tetr2ndOrderNodes->SetName ("tetr2ndOrderNodes");
	g->GetCellData ()->AddArray (tetr2ndOrderNodes);
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
	nodeBorderCondId->Delete();
	
	tetr1stOrderNodes->Delete ();
	tetr2ndOrderNodes->Delete ();
	tetrNumber->Delete ();
	
	xgw->SetInput(g);
	xgw->SetFileName(filename.c_str());
	xgw->Update();
	
	xgw->Delete();
	g->Delete();
	pts->Delete();
	tetra->Delete();
}

bool gcm::VTK2SnapshotWriter::shouldSnapshot(CalcNode& node, TetrMeshSecondOrder* mesh)
{
	// Case 1 - unused node
	if( ! node.isUsed() )
		return false;
	
	// Case 2 - remote ones that have connections only with remote ones
	// Disable it, since VTK2 is used for 'dumps' only (not for 'snapshots') 
	// and we need remote nodes in dumps for debug purposes
	/*if( node.isRemote() )
	{
		int count = 0;
		// Check tetrahedrons it is a member of
		for(unsigned j = 0; j < node.elements->size(); j++)
		{
			TetrSecondOrder& tetr = mesh->getTetr2( node.elements->at(j) );
			// Check verticles
			for(int k = 0; k < 4; k++)
			{
				// If it is local - count++
				if( mesh->getNode( tetr.verts[k] )->isLocal() )
					count++;
			}
			// Check verticles
			for(int k = 0; k < 6; k++)
			{
				// If it is local - count++
				if( mesh->getNode( tetr.addVerts[k] )->isLocal() )
					count++;
			}
		}
		// If remote node is NOT connected with at least one local - it is unused one
		if(count == 0)
			return false;
	}*/
	
	return true;
}
