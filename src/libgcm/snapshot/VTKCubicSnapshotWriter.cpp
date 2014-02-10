#include "snapshot/VTKCubicSnapshotWriter.h"

gcm::VTKCubicSnapshotWriter::VTKCubicSnapshotWriter() {
	INIT_LOGGER("gcm.VTKCubicSnapshotWriter");
	fname = string ("snap_mesh_%m_cpu_%z_step_%n.vts");
}

gcm::VTKCubicSnapshotWriter::VTKCubicSnapshotWriter(const char* snapName) {
	INIT_LOGGER("gcm.VTKCubicSnapshotWriter");
	fname = string (snapName);
}

string gcm::VTKCubicSnapshotWriter::getType() {
	return "VTKCubicSnapshotWriter";
}

void gcm::VTKCubicSnapshotWriter::dump(Mesh* mesh, int step)
{
	// TODO - check if the mesh is compatible
	dumpVTK(getFileName(MPI::COMM_WORLD.Get_rank(), step, mesh->getId()), (BasicCubicMesh*)mesh, step);
}

void gcm::VTKCubicSnapshotWriter::dumpVTK(string filename, BasicCubicMesh *mesh, int step)
{
	vtkSmartPointer<vtkStructuredGrid> structuredGrid = vtkSmartPointer<vtkStructuredGrid>::New();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	
	vtkDoubleArray *vel = vtkDoubleArray::New();
	vel->SetNumberOfComponents(3);
	vtkDoubleArray *crack = vtkDoubleArray::New();
	crack->SetNumberOfComponents(3);
	vtkDoubleArray *sxx = vtkDoubleArray::New();
	vtkDoubleArray *sxy = vtkDoubleArray::New();
	vtkDoubleArray *sxz = vtkDoubleArray::New();
	vtkDoubleArray *syy = vtkDoubleArray::New();
	vtkDoubleArray *syz = vtkDoubleArray::New();
	vtkDoubleArray *szz = vtkDoubleArray::New();
	vtkDoubleArray *compression = vtkDoubleArray::New();
	vtkDoubleArray *tension = vtkDoubleArray::New();
	vtkDoubleArray *shear = vtkDoubleArray::New();
	vtkDoubleArray *deviator = vtkDoubleArray::New();
	vtkIntArray    *matId = vtkIntArray::New();
	vtkDoubleArray *rho = vtkDoubleArray::New();
	vtkIntArray    *borderState = vtkIntArray::New();
	vtkIntArray    *contactState = vtkIntArray::New();
	vtkIntArray    *mpiState = vtkIntArray::New();
	vtkIntArray	   *nodeErrorFlags = vtkIntArray::New ();
	
	CalcNode* node;
	float v[3];
	float c[3];
	
	for(int i = 0; i < mesh->getNodesNumber(); i++)
	{
		node = mesh->getNodeByLocalIndex(i);
		points->InsertNextPoint( node->coords[0], node->coords[1], node->coords[2] );
		
		v[0] = node->values[0];	v[1] = node->values[1];	v[2] = node->values[2];
		memcpy(c, node->getCrackDirection(), 3*sizeof(float));
		vel->InsertNextTuple(v);
		crack->InsertNextTuple(c);
		sxx->InsertNextValue( node->values[3] );
		sxy->InsertNextValue( node->values[4] );
		sxz->InsertNextValue( node->values[5] );
		syy->InsertNextValue( node->values[6] );
		syz->InsertNextValue( node->values[7] );
		szz->InsertNextValue( node->values[8] );
		compression->InsertNextValue( node->getCompression() );
		tension->InsertNextValue( node->getTension() );
		shear->InsertNextValue( node->getShear() );
		deviator->InsertNextValue( node->getDeviator() );
		matId->InsertNextValue( node->getMaterialId() );
		rho->InsertNextValue( node->getRho() );
		borderState->InsertNextValue( node->isBorder() ? ( node->isInContact() ? 2 : 1 ) : 0 );
		contactState->InsertNextValue(node->getContactConditionId());
		mpiState->InsertNextValue( node->isRemote() ? 1 : 0 );
		nodeErrorFlags->InsertNextValue (node->getErrorFlags());
	}
	
	// Specify the dimensions of the grid
	AABB outline = mesh->getOutline();
	// We suppose that mesh is uniform
	float meshH = mesh->getMinH();
	int meshSize = 1 + (outline.maxX - outline.minX + meshH * 0.1) / meshH;
	structuredGrid->SetDimensions(meshSize,meshSize,meshSize);
	structuredGrid->SetPoints(points);

	vel->SetName("velocity");
	crack->SetName("crack");
	sxx->SetName("sxx");
	sxy->SetName("sxy");
	sxz->SetName("sxz");
	syy->SetName("syy");
	syz->SetName("syz");
	szz->SetName("szz");
	compression->SetName("compression");
	tension->SetName("tension");
	shear->SetName("shear");
	deviator->SetName("deviator");
	matId->SetName("materialID");
	rho->SetName("rho");
	borderState->SetName("borderState");
	contactState->SetName("contactState");
	mpiState->SetName("mpiState");
	nodeErrorFlags->SetName ("errorFlags");
	
	structuredGrid->GetPointData()->SetVectors(vel);
	structuredGrid->GetPointData()->AddArray(crack);
	structuredGrid->GetPointData()->AddArray(sxx);
	structuredGrid->GetPointData()->AddArray(sxy);
	structuredGrid->GetPointData()->AddArray(sxz);
	structuredGrid->GetPointData()->AddArray(syy);
	structuredGrid->GetPointData()->AddArray(syz);
	structuredGrid->GetPointData()->AddArray(szz);
	structuredGrid->GetPointData()->AddArray(compression);
	structuredGrid->GetPointData()->AddArray(tension);
	structuredGrid->GetPointData()->AddArray(shear);
	structuredGrid->GetPointData()->AddArray(deviator);
	structuredGrid->GetPointData()->AddArray(matId);
	structuredGrid->GetPointData()->AddArray(rho);
	structuredGrid->GetPointData()->AddArray(borderState);
	structuredGrid->GetPointData()->AddArray(contactState);
	structuredGrid->GetPointData()->AddArray(mpiState);
	structuredGrid->GetPointData ()->AddArray (nodeErrorFlags);
	
	vel->Delete();
	crack->Delete();
	sxx->Delete();
	sxy->Delete();
	sxz->Delete();
	syy->Delete();
	syz->Delete();
	szz->Delete();
	compression->Delete();
	tension->Delete();
	shear->Delete();
	deviator->Delete();
	matId->Delete();
	rho->Delete();
	borderState->Delete();
	contactState->Delete();
	mpiState->Delete();
	nodeErrorFlags->Delete ();
	
	// Write file
	vtkSmartPointer<vtkXMLStructuredGridWriter> writer = vtkSmartPointer<vtkXMLStructuredGridWriter>::New();
	writer->SetFileName(filename.c_str());
	writer->SetInput(structuredGrid);
	writer->Write();
}
