#include "mesh/tetr/GeoMeshLoader.h"

string gcm::GeoMeshLoader::getType(){
    return "geo";
}

gcm::GeoMeshLoader::GeoMeshLoader() {
    INIT_LOGGER("gcm.GeoMeshLoader");
    GmshInitialize();
}

gcm::GeoMeshLoader::~GeoMeshLoader() {
}

void gcm::GeoMeshLoader::cleanUp() {
    GmshFinalize();
    for( map<string, bool>::const_iterator itr = createdFiles.begin(); itr != createdFiles.end(); ++itr )
    {
        LOG_DEBUG("Deleting generated file: " << getMshFileName(itr->first));
        remove( getMshFileName(itr->first).c_str() );
    }
}

bool gcm::GeoMeshLoader::isMshFileCreated(Params params)
{
    return ( createdFiles.find( params[PARAM_FILE] ) != createdFiles.end() );
}

string gcm::GeoMeshLoader::getMshFileName(string geoFile)
{
    return geoFile + ".tmp.msh";
}

void gcm::GeoMeshLoader::createMshFile(Params params)
{
    if( engine->getNumberOfWorkers() > 1 )
    {
        if( engine->getRank() != 0 )
        {
            MPI::COMM_WORLD.Barrier();
            //mshFileCreated = true;
            return;
        }
    }
    /*
     * TODO@ashevtsov: I don't really understand the meaning of all these options, values
     * have been guessed to get a mesh with acceptable tetrahedra sizes.
     * In future need to undestand GMsh meshing algorithms and set these options correctly.
     */
    float tetrSize = params.count ("tetrSize") > 0 ? atof (params.at ("tetrSize").c_str ()) : 1.0;
    LOG_DEBUG("loadGeoScriptFile (" << engine->getFileFolderLookupService().lookupFile(params[PARAM_FILE])
                                    << "): will mesh with H = " << tetrSize);
    GmshSetOption ("General", "Terminal", 1.0);
    GmshSetOption ("General", "Verbosity", engine->getGmshVerbosity());
    GmshSetOption ("Mesh", "CharacteristicLengthMin", tetrSize);
    GmshSetOption ("Mesh", "CharacteristicLengthMax", tetrSize);
    GmshSetOption ("Mesh", "Optimize", 1.0);

    GModel gmshModel;
    gmshModel.setFactory ("Gmsh");
    gmshModel.readGEO (engine->getFileFolderLookupService().lookupFile(params[PARAM_FILE]));
    LOG_INFO("Creating mesh using gmsh library");
    gmshModel.mesh (3);
    gmshModel.writeMSH (getMshFileName(params[PARAM_FILE]));
    //mshFileCreated = true;
    createdFiles[ params[PARAM_FILE] ] = true;

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
    if( ! isMshFileCreated(params) )
        createMshFile(params);
    MshTetrFileReader* reader = new MshTetrFileReader();
    reader->readFile(getMshFileName(params[PARAM_FILE]), mesh, dispatcher, mesh->getBody()->getEngine()->getRank(), false);
    delete reader;

    mesh->preProcess();
}

void gcm::GeoMeshLoader::preLoadMesh(Params params, AABB* scene, int& sliceDirection, int& numberOfNodes) {
    if (params.find(PARAM_FILE) == params.end()) {
        THROW_INVALID_ARG("Msh file name was not provided");
    }
    if( ! isMshFileCreated(params) )
        createMshFile(params);
    MshTetrFileReader* reader = new MshTetrFileReader();
    reader->preReadFile(getMshFileName(params[PARAM_FILE]), scene, sliceDirection, numberOfNodes);
    delete reader;
}
