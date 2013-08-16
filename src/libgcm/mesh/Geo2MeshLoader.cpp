#include "Geo2MeshLoader.h"

string gcm::Geo2MeshLoader::getType(){
	return "geo2";
}

bool gcm::Geo2MeshLoader::isMshFileCreated(Params params)
{
	return ( createdFiles.find( params[PARAM_FILE] ) != createdFiles.end() );
}

gcm::Geo2MeshLoader::Geo2MeshLoader() {
	INIT_LOGGER("gcm.Geo2MeshLoader");
	GmshInitialize();
}

gcm::Geo2MeshLoader::~Geo2MeshLoader() {
}

void gcm::Geo2MeshLoader::cleanUp() {
	GmshFinalize();
	for( map<string, bool>::const_iterator itr = createdFiles.begin(); itr != createdFiles.end(); ++itr )
	{
		LOG_DEBUG("Deleting generated file: " << getMshFileName(itr->first));
		remove( getMshFileName(itr->first).c_str() );
		LOG_DEBUG("Deleting generated file: " << getVtkFileName(itr->first));
		remove( getVtkFileName(itr->first).c_str() );
	}
}

string gcm::Geo2MeshLoader::getMshFileName(string geoFile)
{
	return geoFile + ".tmp.msh";
}

string gcm::Geo2MeshLoader::getVtkFileName(string geoFile)
{
	return geoFile + ".tmp.vtu";
}

void gcm::Geo2MeshLoader::createMshFile(Params params)
{
	if( engine->getNumberOfWorkers() > 1 )
	{
		if( engine->getRank() != 0 )
		{
			MPI::COMM_WORLD.Barrier();
			//mshFileCreated = true;
			createdFiles[ params[PARAM_FILE] ] = true;
			return;
		}
	}
	/*
	 * TODO@ashevtsov: I don't really understand the meaning of all these options, values
	 * have been guessed to get a mesh with acceptable tetrahedra sizes.
	 * In future need to undestand GMsh meshing algorithms and set these options correctly.
	 */
	float tetrSize = params.count ("tetrSize") > 0 ? atof (params.at ("tetrSize").c_str ()) : 1.0;
	LOG_DEBUG("loadGeoScriptFile (" << engine->getFileLookupService().lookupFile(params[PARAM_FILE]) << "): will mesh with H = " << tetrSize);
	GmshSetOption ("General", "Terminal", 1.0);
	GmshSetOption ("Mesh", "CharacteristicLengthMin", tetrSize);
	GmshSetOption ("Mesh", "CharacteristicLengthMax", tetrSize);
	GmshSetOption ("Mesh", "Optimize", 1.0);

	GModel gmshModel;
	gmshModel.setFactory ("Gmsh");
	gmshModel.readGEO (engine->getFileLookupService().lookupFile(params[PARAM_FILE]));
	gmshModel.mesh (3);
	gmshModel.writeMSH (getMshFileName(params[PARAM_FILE]));
	//mshFileCreated = true;
	createdFiles[ params[PARAM_FILE] ] = true;
	
	if( engine->getNumberOfWorkers() > 1 )
		MPI::COMM_WORLD.Barrier();
}

void gcm::Geo2MeshLoader::loadMesh(Params params, TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher)
{
	if (params.find(PARAM_FILE) == params.end()) {
		delete mesh; 
		THROW_INVALID_ARG("Geo file name was not provided");
	}
	if( ! isMshFileCreated(params) )
		createMshFile(params);
	
	IBody* body = mesh->getBody();
	IEngine* engine = body->getEngine();
	if( engine->getRank() == 0 )
	{
		LOG_DEBUG("Worker 0 started generating second order mesh from first order msh file");
		TetrMeshFirstOrder* foMesh = new TetrMeshFirstOrder();
		foMesh->setBody(body);
		
		TetrMeshSecondOrder* soMesh = new TetrMeshSecondOrder();
		soMesh->setBody(body);
		
		AABB scene;
		GCMDispatcher* myDispatcher = new DummyDispatcher();
		myDispatcher->setEngine(engine);
		preLoadMesh(params, &scene);
		myDispatcher->prepare(1, &scene);
		
		MshTetrFileReader* reader = new MshTetrFileReader();
		reader->readFile(getMshFileName(params[PARAM_FILE]), foMesh, myDispatcher, engine->getRank(), true);
		soMesh->copyMesh(foMesh);
		soMesh->preProcess();
		
		VTK2SnapshotWriter* sw = new VTK2SnapshotWriter();
		sw->setFileName(getVtkFileName(params[PARAM_FILE]));
		sw->dump(soMesh, -1);
		
		delete sw;
		delete reader;
		delete foMesh;
		delete soMesh;
		LOG_DEBUG("Worker 0 completed generating second order mesh");
	}
	
	MPI::COMM_WORLD.Barrier();
	
	LOG_DEBUG("Starting reading mesh");
	Vtu2TetrFileReader* reader = new Vtu2TetrFileReader();
	reader->readFile(getVtkFileName(params[PARAM_FILE]), mesh, dispatcher, engine->getRank());
	delete reader;
	
	mesh->preProcess();
}

void gcm::Geo2MeshLoader::preLoadMesh(Params params, AABB* scene) {
	if (params.find(PARAM_FILE) == params.end()) {
		THROW_INVALID_ARG("Msh file name was not provided");
	}
	if( ! isMshFileCreated(params) )
		createMshFile(params);
	MshTetrFileReader* reader = new MshTetrFileReader();
	reader->preReadFile(getMshFileName(params[PARAM_FILE]), scene);
	delete reader;
}