#include "Msh2MeshLoader.h"
#include "MshMeshLoader.h"

string gcm::Msh2MeshLoader::getType(){
	return "msh2";
}

gcm::Msh2MeshLoader::Msh2MeshLoader() {
	INIT_LOGGER("gcm.Msh2MeshLoader");
	vtkFileName = "tmp.vtu";
}

gcm::Msh2MeshLoader::~Msh2MeshLoader() {
}

void gcm::Msh2MeshLoader::cleanUp() {
	LOG_DEBUG("Deleting generated file: " << vtkFileName);
	remove( vtkFileName.c_str() );
}

void gcm::Msh2MeshLoader::loadMesh(Params params, TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher)
{
	if (params.find(PARAM_FILE) == params.end()) {
		delete mesh; 
		THROW_INVALID_ARG("Msh file name was not provided");
	}
	IBody* body = mesh->getBody();
	IEngine* engine = body->getEngine();
	if( engine->getRank() == 0 )
	{
		LOG_DEBUG("Worker 0 started generating second order mesh");
		TetrMeshFirstOrder* foMesh = new TetrMeshFirstOrder();
		foMesh->setNodeFactory(engine->getNodeFactory(body->getRheology()));
		foMesh->setBody(body);
		
		TetrMeshSecondOrder* soMesh = new TetrMeshSecondOrder();
		soMesh->setNodeFactory(engine->getNodeFactory(body->getRheology()));
		soMesh->setBody(body);
		
		AABB scene;
		GCMDispatcher* myDispatcher = new DummyDispatcher();
		myDispatcher->setEngine(engine);
		preLoadMesh(params, &scene);
		myDispatcher->prepare(1, &scene);
		
		MshTetrFileReader* reader = new MshTetrFileReader();
		reader->readFile(params[PARAM_FILE], foMesh, myDispatcher, engine->getRank());
		soMesh->copyMesh(foMesh);
		soMesh->preProcess();
		
		VTK2SnapshotWriter* sw = new VTK2SnapshotWriter();
		sw->setFileName(vtkFileName);
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
	reader->readFile(vtkFileName, mesh, dispatcher, engine->getRank());
	delete reader;
	
	mesh->preProcess();
}

void gcm::Msh2MeshLoader::preLoadMesh(Params params, AABB* scene) {
	if (params.find(PARAM_FILE) == params.end()) {
		THROW_INVALID_ARG("Msh file name was not provided");
	}
	MshTetrFileReader* reader = new MshTetrFileReader();
	reader->preReadFile(params[PARAM_FILE], scene);
	delete reader;
}