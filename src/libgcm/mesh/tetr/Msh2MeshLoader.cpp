#include "libgcm/mesh/tetr/Msh2MeshLoader.h"

#include "libgcm/util/formats/MshTetrFileReader.h"

string gcm::Msh2MeshLoader::getType(){
    return "msh2";
}

gcm::Msh2MeshLoader::Msh2MeshLoader() {
    INIT_LOGGER("gcm.Msh2MeshLoader");
}

gcm::Msh2MeshLoader::~Msh2MeshLoader() {
}

string gcm::Msh2MeshLoader::getVtkFileName(string mshFile)
{
    return mshFile + ".tmp.vtu";
}

void gcm::Msh2MeshLoader::cleanUp() {
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
        foMesh->setBody(body);

        TetrMeshSecondOrder* soMesh = new TetrMeshSecondOrder();
        soMesh->setBody(body);

        int sd, nn;
        AABB scene;
        GCMDispatcher* myDispatcher = new DummyDispatcher();
        myDispatcher->setEngine(engine);
        preLoadMesh(params, &scene, sd, nn);
        myDispatcher->prepare(1, &scene);

        MshTetrFileReader* reader = new MshTetrFileReader();
        reader->readFile(engine->getFileFolderLookupService().lookupFile(params[PARAM_FILE]),
                            foMesh, myDispatcher, engine->getRank(), true);
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
    LOG_DEBUG("Deleting generated file: " << getVtkFileName(params[PARAM_FILE]));
    remove( getVtkFileName(params[PARAM_FILE]).c_str() );

    mesh->preProcess();
}

void gcm::Msh2MeshLoader::preLoadMesh(Params params, AABB* scene, int& sliceDirection, int& numberOfNodes) {
    if (params.find(PARAM_FILE) == params.end()) {
        THROW_INVALID_ARG("Msh file name was not provided");
    }
    MshTetrFileReader* reader = new MshTetrFileReader();
    reader->preReadFile(engine->getFileFolderLookupService().lookupFile(params[PARAM_FILE]), scene, sliceDirection, numberOfNodes);
    delete reader;
}
