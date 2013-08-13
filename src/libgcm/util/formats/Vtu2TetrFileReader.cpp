#include "Vtu2TetrFileReader.h"

gcm::Vtu2TetrFileReader::Vtu2TetrFileReader()
{
	INIT_LOGGER("gcm.Vtu2TetrFileReader");
}

gcm::Vtu2TetrFileReader::~Vtu2TetrFileReader()
{
	
}

void gcm::Vtu2TetrFileReader::preReadFile(string file, AABB* scene)
{
	scene->minX = numeric_limits<float>::infinity();
	scene->minY = numeric_limits<float>::infinity();
	scene->minZ = numeric_limits<float>::infinity();
	scene->maxX = - numeric_limits<float>::infinity();
	scene->maxY = - numeric_limits<float>::infinity();
	scene->maxZ = - numeric_limits<float>::infinity();

	vtkXMLUnstructuredGridReader *xgr = vtkXMLUnstructuredGridReader::New();
	vtkUnstructuredGrid *g = vtkUnstructuredGrid::New();

	xgr->SetFileName(const_cast<char*>(file.c_str()));
	xgr->Update();

	g = xgr->GetOutput();
	LOG_DEBUG("Number of points: " << g->GetNumberOfPoints());
	LOG_DEBUG("Number of cells: " << g->GetNumberOfCells());
	
	for( int i = 0; i < g->GetNumberOfPoints(); i++ )
	{
		double* dp = g->GetPoint(i);

		if( dp[0] < scene->minX )
			scene->minX = dp[0];
		if( dp[0] > scene->maxX )
			scene->maxX = dp[0];
		
		if( dp[1] < scene->minY )
			scene->minY = dp[1];
		if( dp[1] > scene->maxY )
			scene->maxY = dp[1];
		
		if( dp[2] < scene->minZ )
			scene->minZ = dp[2];
		if( dp[2] > scene->maxZ )
			scene->maxZ = dp[2];
	}
	
	//xgr->Delete();
	//g->Delete();
	LOG_DEBUG("File successfylly pre-read.");
}

void gcm::Vtu2TetrFileReader::readFile(string file, TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher, int rank)
{
	readFile(file, mesh, dispatcher, rank, false);
}

void gcm::Vtu2TetrFileReader::readFile(string file, TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher, int rank, bool ignoreDispatcher)
{
	vtkXMLUnstructuredGridReader *xgr = vtkXMLUnstructuredGridReader::New();
	vtkUnstructuredGrid *g = vtkUnstructuredGrid::New();

	xgr->SetFileName(const_cast<char*>(file.c_str()));
	xgr->Update();

	g = xgr->GetOutput();
	LOG_DEBUG("Number of points: " << g->GetNumberOfPoints());
	LOG_DEBUG("Number of cells: " << g->GetNumberOfCells());
	
	double v[3];
	vtkDoubleArray *vel = (vtkDoubleArray*) g->GetPointData()->GetArray("velocity");
	vel->SetNumberOfComponents(3);
	vtkDoubleArray *sxx = (vtkDoubleArray*) g->GetPointData()->GetArray("sxx");
	vtkDoubleArray *sxy = (vtkDoubleArray*) g->GetPointData()->GetArray("sxy");
	vtkDoubleArray *sxz = (vtkDoubleArray*) g->GetPointData()->GetArray("sxz");
	vtkDoubleArray *syy = (vtkDoubleArray*) g->GetPointData()->GetArray("syy");
	vtkDoubleArray *syz = (vtkDoubleArray*) g->GetPointData()->GetArray("syz");
	vtkDoubleArray *szz = (vtkDoubleArray*) g->GetPointData()->GetArray("szz");
	vtkDoubleArray *la = (vtkDoubleArray*) g->GetPointData()->GetArray("lambda");
	vtkDoubleArray *mu = (vtkDoubleArray*) g->GetPointData()->GetArray("mu");
	vtkDoubleArray *rho = (vtkDoubleArray*) g->GetPointData()->GetArray("rho");
	vtkIntArray *nodeNumber = (vtkIntArray*) g->GetPointData ()->GetArray("nodeNumber");
	vtkIntArray *publicFlags = (vtkIntArray*) g->GetPointData ()->GetArray("publicFlags");
	vtkIntArray *privateFlags = (vtkIntArray*) g->GetPointData ()->GetArray("privateFlags");
	vtkIntArray *nodeBorderCalcId = (vtkIntArray*) g->GetPointData ()->GetArray("borderCalcId");
	
	vector<ElasticNode*>* nodes = new vector<ElasticNode*>;
	for( int i = 0; i < g->GetNumberOfPoints(); i++ )
	{
		double* dp = g->GetPoint(i);
		if( ignoreDispatcher || dispatcher->isMine( dp ) )
		{
			ElasticNode* node = new ElasticNode();
			node->number = nodeNumber->GetValue(i);
			node->coords[0] = dp[0];
			node->coords[1] = dp[1];
			node->coords[2] = dp[2];
			vel->GetTupleValue(i, v);
			node->vx = v[0];
			node->vy = v[1];
			node->vz = v[2];
			node->sxx = sxx->GetValue(i);
			node->sxy = sxy->GetValue(i);
			node->sxz = sxz->GetValue(i);
			node->syy = syy->GetValue(i);
			node->syz = syz->GetValue(i);
			node->szz = szz->GetValue(i);
			node->la = la->GetValue(i);
			node->mu = mu->GetValue(i);
			node->rho = rho->GetValue(i);
			node->setPublicFlags( publicFlags->GetValue(i) );
			node->setPrivateFlags( privateFlags->GetValue(i) );
			node->setBorderConditionId( nodeBorderCalcId->GetValue(i) );
			if( !ignoreDispatcher )
				node->setPlacement(Local);
			nodes->push_back( node );
		}
	}
	LOG_DEBUG("Finished reading nodes");
	LOG_DEBUG("There are " << nodes->size() << " local nodes");
	
	mesh->createNodes( nodes->size() );
	for(unsigned int i = 0; i < nodes->size(); i++)
	{
		mesh->addNode( nodes->at(i) );
	}
	for(unsigned int i = 0; i < nodes->size(); i++)
	{
		delete( nodes->at(i));
	}
	nodes->clear();
	delete nodes;
	
	vtkIntArray* tetr2ndOrderNodes = (vtkIntArray*) g->GetCellData ()->GetArray ("tetr2ndOrderNodes");
	assert (tetr2ndOrderNodes->GetNumberOfComponents () == 6);
	vtkIntArray* tetr1stOrderNodes = (vtkIntArray*) g->GetCellData ()->GetArray ("tetr1stOrderNodes");
	vtkIntArray* tetrNumber = (vtkIntArray*) g->GetCellData ()->GetArray ("tetrNumber");
	assert (tetr1stOrderNodes->GetNumberOfComponents () == 4);
	vector<TetrSecondOrder*>* tetrs = new vector<TetrSecondOrder*>;
	
	TetrSecondOrder new_tetr;
	for( int i = 0; i < g->GetNumberOfCells(); i++ )
	{
		new_tetr.number = tetrNumber->GetValue(i);
		tetr1stOrderNodes->GetTupleValue (i, new_tetr.verts);
		tetr2ndOrderNodes->GetTupleValue (i, new_tetr.addVerts);
		
		/*vtkTetra *vt = (vtkTetra*) g->GetCell(i);
		
		int vert[4];
		vert[0] = vt->GetPointId(0);
		vert[1] = vt->GetPointId(1);
		vert[2] = vt->GetPointId(2);
		vert[3] = vt->GetPointId(3);*/
		
		if( mesh->getNode(new_tetr.verts[0]) != NULL 
					|| mesh->getNode(new_tetr.verts[1]) != NULL 
					|| mesh->getNode(new_tetr.verts[2]) != NULL 
					|| mesh->getNode(new_tetr.verts[3]) != NULL )
				tetrs->push_back( new TetrSecondOrder( new_tetr.number, new_tetr.verts, new_tetr.addVerts ) );
	}
		
	LOG_DEBUG("File contains " << g->GetNumberOfCells() << " tetrs");
	LOG_DEBUG("There are " << tetrs->size() << " local tetrs");
	
	map<int,int> remoteNodes;
	mesh->createTetrs( tetrs->size() );
	for(unsigned int i = 0; i < tetrs->size(); i++)
	{
		TetrSecondOrder* tetr = tetrs->at(i);
		mesh->addTetr2( tetr );
		for(int j = 0; j < 4; j++)
			if( mesh->getNode( tetr->verts[j] ) == NULL )
				remoteNodes[tetr->verts[j]] = i;
		for(int j = 0; j < 6; j++)
			if( mesh->getNode( tetr->addVerts[j] ) == NULL )
				remoteNodes[tetr->addVerts[j]] = i;
	}
	for(unsigned int i = 0; i < tetrs->size(); i++)
	{
		delete( tetrs->at(i) );
	}
	tetrs->clear();
	delete tetrs;
	
	LOG_DEBUG("Finished reading elements");
	
	LOG_DEBUG("Reading required remote nodes");
	LOG_DEBUG("We expect " << remoteNodes.size() << " nodes" );
	int remoteNodesCount = 0;

	ElasticNode tmpNode;
	for( int i = 0; i < g->GetNumberOfPoints(); i++ )
	{
		if( remoteNodes.find( i ) != remoteNodes.end() )
		{
			double* dp = g->GetPoint(i);
			tmpNode.number = nodeNumber->GetValue(i);
			tmpNode.coords[0] = dp[0];
			tmpNode.coords[1] = dp[1];
			tmpNode.coords[2] = dp[2];
			vel->GetTupleValue(i, v);
			tmpNode.vx = v[0];
			tmpNode.vy = v[1];
			tmpNode.vz = v[2];
			tmpNode.sxx = sxx->GetValue(i);
			tmpNode.sxy = sxy->GetValue(i);
			tmpNode.sxz = sxz->GetValue(i);
			tmpNode.syy = syy->GetValue(i);
			tmpNode.syz = syz->GetValue(i);
			tmpNode.szz = szz->GetValue(i);
			tmpNode.la = la->GetValue(i);
			tmpNode.mu = mu->GetValue(i);
			tmpNode.rho = rho->GetValue(i);
			tmpNode.setPublicFlags( publicFlags->GetValue(i) );
			tmpNode.setPrivateFlags( privateFlags->GetValue(i) );			
			tmpNode.setBorderConditionId( nodeBorderCalcId->GetValue(i) );
			tmpNode.setPlacement(Remote);
			mesh->addNode(&tmpNode);
			remoteNodesCount++;
		}
	}
	
	LOG_DEBUG("Read " << remoteNodesCount << " remote nodes");
	
	LOG_DEBUG("Finished reading nodes");

	LOG_DEBUG("File successfylly read.");
	
	LOG_DEBUG("There are " << mesh->getNodesNumber() << " nodes is the mesh");
	
	LOG_DEBUG("Checking tetrs and nodes");
	
	for( int i = 0; i < mesh->getTetrsNumber(); i++ )
	{
		TetrSecondOrder* tetr = mesh->getTetr2ByLocalIndex(i);
		for (int j = 0; j < 4; j++)
			if ( mesh->getNode(tetr->verts[j]) == NULL )
				LOG_WARN("Bad vert: " << tetr->verts[j]);
		for (int j = 0; j < 6; j++)
			if ( mesh->getNode(tetr->addVerts[j]) == NULL )
				LOG_WARN("Bad vert: " << tetr->addVerts[j]);
	}
	
	//xgr->Delete();
	//g->Delete();
}