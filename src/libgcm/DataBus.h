#ifndef GCM_DATABUS_H
#define    GCM_DATABUS_H

#include "libgcm/Interfaces.h"
#include "libgcm/GCMDispatcher.h"
#include "libgcm/Logging.h"

using namespace std;
using namespace gcm;

namespace gcm
{
    class TetrMeshSecondOrder;

    class DataBus {
    public:
        DataBus();
        ~DataBus();
        void setEngine(IEngine* engine);
        IEngine* getEngine();

        void syncTimeStep(float* tau);
        void syncNodes(float tau);
        void syncOutlines();
        void syncMissedNodes(Mesh* _mesh, float tau);
        void transferNodes(TetrMeshSecondOrder* mesh, vector<AABB>* _reqZones);
    private:
        void createStaticTypes();
        void syncNodes(int bodyNum, float tau);
        void createDynamicTypes(int bodyNum);
        USE_LOGGER;
        IEngine* engine;
        MPI::Datatype **MPI_NODE_TYPES;
        vector<int> **local_numbers;
        int numberOfWorkers;
        int rank;

        // MPI tags
        static const int TAG_SYNC_NODE         =  2000;
        static const int TAG_SYNC_FACES_REQ_Z  =  7;
        static const int TAG_SYNC_FACES_REQ_I  =  20;
        static const int TAG_SYNC_FACES_RESP   =  19;
        static const int TAG_SYNC_FACES_F_RESP =  1000;
        static const int TAG_SYNC_FACES_N_RESP =  5000;
        static const int TAG_SYNC_TETRS_REQ    = 12;
        static const int TAG_SYNC_TETRS_REQ_I  = 19;
        static const int TAG_SYNC_TETRS_T_RESP = 13;
        static const int TAG_SYNC_TETRS_N_RESP = 14;
        static const int TAG_SYNC_TETRS_I_RESP = 21;
        static const int TAG_SYNC_NODE_TYPES   = 17;
        static const int TAG_SYNC_NODE_TYPES_I = 18;
        static const int TAG_GET_TETRS_N = 10000;
        static const int TAG_GET_TETRS_T = 20000;

    public:
        // MPI types
        MPI::Datatype MPI_ELNODE;
        MPI::Datatype MPI_ELNODE_NUMBERED;
        MPI::Datatype MPI_FACE_NUMBERED;
        MPI::Datatype MPI_TETR_NUMBERED;
        MPI::Datatype MPI_MESH_OUTLINE;
        MPI::Datatype MPI_OUTLINE;

        // return code on termination
        static const int MPI_CODE_TERMINATED = 0;
    };
};

#endif    /* GCM_DATABUS_H */

