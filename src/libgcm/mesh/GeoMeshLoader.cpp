#include "GeoMeshLoader.h"

string gcm::GeoMeshLoader::getType(){
	return "geo";
}

gcm::GeoMeshLoader::GeoMeshLoader() {
	INIT_LOGGER("gcm.GeoMeshLoader");
	GmshInitialize();
	mshFileCreated = false;
	mshFileName = "tmp.msh";
}

gcm::GeoMeshLoader::~GeoMeshLoader() {
}

void gcm::GeoMeshLoader::cleanUp() {
	GmshFinalize();
	LOG_DEBUG("Deleting generated file: " << mshFileName);
	remove( mshFileName.c_str() );
}

void gcm::GeoMeshLoader::createMshFile(Params params)
{
	if( engine->getNumberOfWorkers() > 1 )
	{
		if( engine->getRank() != 0 )
		{
			MPI::COMM_WORLD.Barrier();
			mshFileCreated = true;
			return;
		}
	}
	/*
	 * TODO@ashevtsov: I don't really understand the meaning of all these options, values
	 * have been guessed to get a mesh with acceptable tetrahedra sizes.
	 * In future need to undestand GMsh meshing algorithms and set these options correctly.
	 */
	float tetrSize = params.count ("tetrSize") > 0 ? atof (params.at ("tetrSize").c_str ()) : 1.0;
	LOG_DEBUG("loadGeoScriptFile (" << params[PARAM_FILE] << "): will mesh with H = " << tetrSize);
	GmshSetOption ("General", "Terminal", 1.0);
	GmshSetOption ("Mesh", "CharacteristicLengthMin", tetrSize);
	GmshSetOption ("Mesh", "CharacteristicLengthMax", tetrSize);
	GmshSetOption ("Mesh", "Optimize", 1.0);

	GModel gmshModel;
	gmshModel.setFactory ("Gmsh");
	gmshModel.readGEO (engine->getFileLookupService().lookupFile(params[PARAM_FILE]));
	gmshModel.mesh (3);
	gmshModel.writeMSH (mshFileName);
	mshFileCreated = true;
	
	if( engine->getNumberOfWorkers() > 1 )
		MPI::COMM_WORLD.Barrier();
}

void gcm::GeoMeshLoader::loadMesh(Params params, TetrMeshFirstOrder* mesh, GCMDispatcher* dispatcher)
{
	THROW_INVALID_ARG("First order mesh is not supported now. Please use second order mesh and geo2 loader.");
	if (params.find(PARAM_FILE) == params.end()) {
		delete mesh; 
		THROW_INVALID_ARG("Geo file name was not provided");
	}
	if( ! mshFileCreated )
		createMshFile(params);
	MshTetrFileReader* reader = new MshTetrFileReader();
	reader->readFile(mshFileName, mesh, dispatcher, mesh->getBody()->getEngine()->getRank());
	delete reader;
	
	mesh->preProcess();
}

void gcm::GeoMeshLoader::preLoadMesh(Params params, AABB* scene) {
	if (params.find(PARAM_FILE) == params.end()) {
		THROW_INVALID_ARG("Msh file name was not provided");
	}
	if( ! mshFileCreated )
		createMshFile(params);
	MshTetrFileReader* reader = new MshTetrFileReader();
	reader->preReadFile(mshFileName, scene);
	delete reader;
}