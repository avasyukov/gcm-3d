#include "libgcm/DataBus.hpp"

#include <vector>

#include "libgcm/mesh/tetr/TetrMeshSecondOrder.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/mesh/Mesh.hpp"
#include "libgcm/Body.hpp"

#define BARRIER(name) \
do { \
    LOG_DEBUG("Entering barrier: " << name); \
    MPI::COMM_WORLD.Barrier(); \
    LOG_DEBUG("Barrier passed: " << name); \
} while(0)

using namespace gcm;
using std::vector;
using std::map;

DataBus::DataBus() {
    INIT_LOGGER("gcm.DataBus");
    MPI_NODE_TYPES = NULL;
    local_numbers = NULL;
    rank = MPI::COMM_WORLD.Get_rank();
    numberOfWorkers = MPI::COMM_WORLD.Get_size();
    createStaticTypes();
}

DataBus::~DataBus() {
    if (MPI_NODE_TYPES != NULL) {
        LOG_TRACE("Cleaning old types");
        for (int i = 0; i < numberOfWorkers; i++)
        {
            for (int j = 0; j < numberOfWorkers; j++)
            {
                LOG_TRACE("Cleaning type " << i << " " << j );
                LOG_TRACE("Size " << i << " " << j << " = " << local_numbers[i][j].size());
                if (local_numbers[i][j].size() > 0)
                    MPI_NODE_TYPES[i][j].Free();
            }
        }
        delete[] MPI_NODE_TYPES;
    }

    if (local_numbers != NULL) {
        for (int i = 0; i < numberOfWorkers; i++)
            delete[] local_numbers[i];
        delete[] local_numbers;
    }
}

void DataBus::syncTimeStep(float* tau) {
    BARRIER("DataBus::syncTimeStep#1");
    MPI::COMM_WORLD.Allreduce(MPI_IN_PLACE, tau, 1, MPI::FLOAT, MPI::MIN);
}

void DataBus::syncNodes(float tau)
{
    for(int i = 0; i < Engine::getInstance().getNumberOfBodies(); i++ )
        syncNodes(i, tau);
}

void DataBus::syncNodes(int bodyNum, float tau)
{
    if( numberOfWorkers == 1 )
        return;

    LOG_DEBUG("Working with body " << bodyNum);
    LOG_DEBUG("Creating dynamic types");
    createDynamicTypes(bodyNum);
    LOG_DEBUG("Creating dynamic types done");

    vector<MPI::Request> reqs;
    BARRIER("DataBus::syncNodes#1");
    LOG_DEBUG("Starting nodes sync");

    Body* body = Engine::getInstance().getBody(bodyNum);//ById( engine.getDispatcher()->getMyBodyId() );
    TetrMeshSecondOrder* mesh = (TetrMeshSecondOrder*)body->getMeshes();

    for (int i = 0; i < numberOfWorkers; i++)
        for (int j = 0; j < numberOfWorkers; j++)
            if (local_numbers[i][j].size() && i != rank)
            {
                LOG_DEBUG("Sending nodes from zone " << j << " to zone " << i);
                reqs.push_back(
                    MPI::COMM_WORLD.Isend(
                        &mesh->nodes[0],
                        1,
                        MPI_NODE_TYPES[i][j],
                        i,
                        TAG_SYNC_NODE+100*i+j
                    )
                );
            }

    for (int i = 0; i < numberOfWorkers; i++)
        for (int j = 0; j < numberOfWorkers; j++)
            if (local_numbers[i][j].size() && i == rank)
            {
                LOG_DEBUG("Receiving nodes from zone " << j << " to zone " << i);
                reqs.push_back(
                    MPI::COMM_WORLD.Irecv(
                        &mesh->nodes[0],
                        1,
                        MPI_NODE_TYPES[i][j],
                        j,
                        TAG_SYNC_NODE+100*i+j
                    )
                );
            }

    MPI::Request::Waitall(reqs.size(), &reqs[0]);
    BARRIER("DataBus::syncNodes#2");

    LOG_DEBUG("Nodes sync done");

    /*for(int i = 0; i < engine.getNumberOfBodies(); i++)
    {
        Body* body = engine.getBody(i);
        TetrMeshSecondOrder* mesh = (TetrMeshSecondOrder*)body->getMeshes();
        LOG_DEBUG( "Syncing remote data for mesh " << i );
        float reqH = tau * mesh->getMaxLambda();

        LOG_DEBUG( "Mesh sync done" );
    }*/
}

void DataBus::createStaticTypes()
{
    AABB outlines[2];
    TetrMeshSecondOrder meshes[2];
    TriangleSecondOrder faces[2];
    TetrSecondOrder tetrs[2];

    MPI::Datatype outl_types[] = {
        MPI::LB,
        MPI::FLOAT,
        MPI::FLOAT,
        MPI::UB
    };

    int outl_lengths[] = {
        1,
        3,
        3,
        1
    };

    MPI::Aint outl_displacements[] = {
        MPI::Get_address(&outlines[0]),
        MPI::Get_address(&outlines[0].min_coords[0]),
        MPI::Get_address(&outlines[0].max_coords[0]),
        MPI::Get_address(&outlines[1])
    };

    for (int i = 3; i >=0; i--)
        outl_displacements[i] -= MPI::Get_address(&outlines[0]);

    MPI_OUTLINE = MPI::Datatype::Create_struct(
        4,
        outl_lengths,
        outl_displacements,
        outl_types
    );

    MPI::Datatype mesh_outl_types[] = {
        MPI::LB,
        MPI::FLOAT,
        MPI::FLOAT,
        MPI::UB
    };

    int mesh_outl_lengths[] = {
        1,
        3,
        3,
        1
    };

    MPI::Aint mesh_outl_displacements[] = {
        MPI::Get_address(&meshes[0]),
        MPI::Get_address(&meshes[0].outline.min_coords[0]),
        MPI::Get_address(&meshes[0].outline.max_coords[0]),
        MPI::Get_address(&meshes[1])
    };

    for (int i = 3; i >=0; i--)
        mesh_outl_displacements[i] -= mesh_outl_displacements[0];

    MPI_MESH_OUTLINE = MPI::Datatype::Create_struct(
        4,
        mesh_outl_lengths,
        mesh_outl_displacements,
        mesh_outl_types
    );

    MPI::Datatype face_types[] =
    {
        MPI::LB,
        MPI::INT,
        MPI::INT,
        MPI::UB
    };

    int face_lens[] = {
        1,
        1,
        3,
        1
    };

    MPI::Aint face_displ[] = {
        MPI::Get_address(&faces[0]),
        MPI::Get_address(&faces[0].number),
        MPI::Get_address(&faces[0].verts[0]),
        MPI::Get_address(&faces[1])
    };

    for (int i = 3; i >= 0; i--)
        face_displ[i] -= face_displ[0];

    MPI_FACE_NUMBERED = MPI::Datatype::Create_struct(
        4,
        face_lens,
        face_displ,
        face_types
    );

    MPI::Datatype tetr_types[] =
    {
        MPI::LB,
        MPI::INT,
        MPI::INT,
        MPI::INT,
        MPI::UB
    };

    int tetr_lens[] = {
        1,
        1,
        4,
        6,
        1
    };

    MPI::Aint tetr_displ[] = {
        MPI::Get_address(&tetrs[0]),
        MPI::Get_address(&tetrs[0].number),
        MPI::Get_address(&tetrs[0].verts[0]),
        MPI::Get_address(&tetrs[0].addVerts[0]),
        MPI::Get_address(&tetrs[1])
    };

    for (int i = 4; i >= 0; i--)
        tetr_displ[i] -= tetr_displ[0];

    MPI_TETR_NUMBERED = MPI::Datatype::Create_struct(
        5,
        tetr_lens,
        tetr_displ,
        tetr_types
    );

    CalcNode elnodes[2];
    MPI::Datatype elnode_types[] = {
        MPI::LB,
        MPI::FLOAT,
        MPI::FLOAT,
        MPI::FLOAT,
        MPI::UNSIGNED_CHAR,
        MPI::UNSIGNED_CHAR,
        MPI::UNSIGNED,
        MPI::UNSIGNED_CHAR,
        MPI::UB
    };

    int elnode_lens[] = {
        1,
        9,
        3,
        1,
        1,
        1,
        1,
        1,
        1
    };

    MPI::Aint elnode_displs[] = {
        MPI::Get_address(&elnodes[0]),
        MPI::Get_address(&elnodes[0].values[0]),
        MPI::Get_address(&elnodes[0].coords[0]),
        MPI::Get_address(&elnodes[0].rho),
        MPI::Get_address(&elnodes[0].bodyId),
        MPI::Get_address(&elnodes[0].materialId),
        MPI::Get_address(&elnodes[0].publicFlags),
        MPI::Get_address(&elnodes[0].borderConditionId),
        MPI::Get_address(&elnodes[1])
    };
    for (int i = 8; i >= 0; i--)
        elnode_displs[i] -= elnode_displs[0];

    MPI_ELNODE = MPI::Datatype::Create_struct(
        9,
        elnode_lens,
        elnode_displs,
        elnode_types
    );
    MPI_ELNODE.Commit();

    MPI::Datatype elnoden_types[] = {
        MPI::LB,
        MPI::INT,
        MPI::FLOAT,
        MPI::FLOAT,
        MPI::FLOAT,
        MPI::UNSIGNED_CHAR,
        MPI::UNSIGNED_CHAR,
        MPI::UNSIGNED,
        MPI::UNSIGNED_CHAR,
        MPI::UB
    };

    int elnoden_lens[] = {
        1,
        1,
        9,
        3,
        1,
        1,
        1,
        1,
        1,
        1
    };

    MPI::Aint elnoden_displs[] = {
        MPI::Get_address(&elnodes[0]),
        MPI::Get_address(&elnodes[0].number),
        MPI::Get_address(&elnodes[0].values[0]),
        MPI::Get_address(&elnodes[0].coords[0]),
        MPI::Get_address(&elnodes[0].rho),
        MPI::Get_address(&elnodes[0].bodyId),
        MPI::Get_address(&elnodes[0].materialId),
        MPI::Get_address(&elnodes[0].publicFlags),
        MPI::Get_address(&elnodes[0].borderConditionId),
        MPI::Get_address(&elnodes[1])
    };
    for (int i = 9; i >= 0; i--)
        elnoden_displs[i] -= elnoden_displs[0];

    MPI_ELNODE_NUMBERED = MPI::Datatype::Create_struct(
        10,
        elnoden_lens,
        elnoden_displs,
        elnoden_types
    );

    MPI_ELNODE_NUMBERED.Commit();
    MPI_FACE_NUMBERED.Commit();
    MPI_TETR_NUMBERED.Commit();
    MPI_MESH_OUTLINE.Commit();
    MPI_OUTLINE.Commit();
}

void DataBus::createDynamicTypes(int bodyNum)
{
    LOG_DEBUG("Building dynamic MPI types for fast node sync");
    auto& engine = Engine::getInstance();
    GCMDispatcher* dispatcher = engine.getDispatcher();
    Body* body = engine.getBody(bodyNum);//ById( engine.getDispatcher()->getMyBodyId() );
    TetrMeshSecondOrder* mesh = (TetrMeshSecondOrder*)body->getMeshes();

    // TODO add more cleanup code here to prevent memory leaks
    if (MPI_NODE_TYPES != NULL) {
        LOG_TRACE("Cleaning old types");
        for (int i = 0; i < numberOfWorkers; i++)
        {
            for (int j = 0; j < numberOfWorkers; j++)
            {
                LOG_TRACE("Cleaning type " << i << " " << j );
                LOG_TRACE("Size " << i << " " << j << " = " << local_numbers[i][j].size());
                if (local_numbers[i][j].size() > 0)
                    MPI_NODE_TYPES[i][j].Free();
            }
        }
        delete[] MPI_NODE_TYPES;
    }

    if (local_numbers != NULL) {
        for (int i = 0; i < numberOfWorkers; i++)
            delete[] local_numbers[i];
        delete[] local_numbers;
    }

    // FIXME - it's overhead
    local_numbers = new vector<int>*[numberOfWorkers];
    vector<int> **remote_numbers = new vector<int>*[numberOfWorkers];
    MPI_NODE_TYPES = new MPI::Datatype*[numberOfWorkers];

    for (int i = 0; i < numberOfWorkers; i++)
    {
        local_numbers[i] = new vector<int>[numberOfWorkers];
        remote_numbers[i] = new vector<int>[numberOfWorkers];
        MPI_NODE_TYPES[i] = new MPI::Datatype[numberOfWorkers];
    }

    BARRIER("DataBus::createDynamicTypes#0");

    // find all remote nodes
    for (int j = 0; j < mesh->getNodesNumber(); j++)
    {
        CalcNode& node = mesh->getNodeByLocalIndex(j);
        if ( node.isRemote() )
        {
            //LOG_DEBUG("N: " << j);
            //LOG_DEBUG("R1: " << j << " " << mesh->getBody()->getId());
            int owner = dispatcher->getOwner(node.coords/*, mesh->getBody()->getId()*/);
            //LOG_DEBUG("R2: " << owner);
            assert_ne(owner, rank );
            local_numbers[rank][owner].push_back( mesh->nodesMap[node.number] );
            remote_numbers[rank][owner].push_back(node.number);
        }
    }

    BARRIER("DataBus::createDynamicTypes#1");

    LOG_DEBUG("Requests prepared:");
    for (int i = 0; i < numberOfWorkers; i++)
        for (int j = 0; j < numberOfWorkers; j++)
            LOG_DEBUG("Request size from #" << i << " to #" << j << ": " << local_numbers[i][j].size());

    // sync types
    unsigned int max_len = 0;
    for (int i = 0; i < numberOfWorkers; i++)
        for (int j = 0; j < numberOfWorkers; j++)
            if (local_numbers[i][j].size() > max_len)
                max_len = local_numbers[i][j].size();

    vector<int> lengths;
    for (unsigned int i = 0; i < max_len; i++)
        lengths.push_back(1);

    int info[3];

    vector<MPI::Request> reqs;

    for (int i = 0; i < numberOfWorkers; i++)
        for (int j = 0; j < numberOfWorkers; j++)
            if (local_numbers[i][j].size() > 0)
            {
                info[0] = remote_numbers[i][j].size();
                info[1] = i;
                info[2] = j;
                MPI_NODE_TYPES[i][j] =  MPI_ELNODE.Create_indexed(
                    local_numbers[i][j].size(),
                    &lengths[0],
                    &local_numbers[i][j][0]
                );
                MPI_NODE_TYPES[i][j].Commit();
                reqs.push_back(
                    MPI::COMM_WORLD.Isend(
                        &remote_numbers[i][j][0],
                        remote_numbers[i][j].size(),
                        MPI::INT,
                        j,
                        TAG_SYNC_NODE_TYPES
                    )
                );
                reqs.push_back(
                    MPI::COMM_WORLD.Isend(
                        info,
                        3,
                        MPI::INT,
                        j,
                        TAG_SYNC_NODE_TYPES_I
                    )
                );
            }

    BARRIER("DataBus::createDynamicTypes#2");

    MPI::Status status;

    while (MPI::COMM_WORLD.Iprobe(MPI::ANY_SOURCE, TAG_SYNC_NODE_TYPES_I, status))
    {
        MPI::COMM_WORLD.Recv(
            info,
            3,
            MPI::INT,
            status.Get_source(),
            TAG_SYNC_NODE_TYPES_I
        );
        local_numbers[info[1]][info[2]].resize(info[0]);
        MPI::COMM_WORLD.Recv(
            &local_numbers[info[1]][info[2]][0],
            info[0],
            MPI::INT,
            status.Get_source(),
            TAG_SYNC_NODE_TYPES
        );
        if (lengths.size() < (unsigned)info[0])
            for (int i = lengths.size(); i < info[0]; i++)
                lengths.push_back(1);
        for(int i = 0; i < info[0]; i++)
            local_numbers[info[1]][info[2]][i] = mesh->nodesMap[ local_numbers[info[1]][info[2]][i] ];
        MPI_NODE_TYPES[info[1]][info[2]] =  MPI_ELNODE.Create_indexed(
            info[0],
            &lengths[0],
            &local_numbers[info[1]][info[2]][0]
        );
        MPI_NODE_TYPES[info[1]][info[2]].Commit();
    }

    MPI::Request::Waitall(reqs.size(), &reqs[0]);
    BARRIER("DataBus::createDynamicTypes#3");

    for (int i = 0 ; i < numberOfWorkers; i++)
        delete[] remote_numbers[i];
    delete[] remote_numbers;
    LOG_DEBUG("Building dynamic MPI types for fast node sync done");
}

void DataBus::syncOutlines()
{
    BARRIER("DataBus::syncOutlines#1");
    AABB* outlines = Engine::getInstance().getDispatcher()->getOutline(0);
    if( outlines == NULL ) {
        THROW_BAD_METHOD("We can't do this because it will cause all MPI routines to freeze");
        return;
    }
    LOG_DEBUG("Syncing outlines");
    MPI::COMM_WORLD.Allgather(
        MPI_IN_PLACE,
        1, MPI_OUTLINE,
        outlines,
        1, MPI_OUTLINE
    );
    LOG_DEBUG("Outlines synced");
}

void DataBus::syncMissedNodes(Mesh* _mesh, float tau)
{
    if( numberOfWorkers == 1 )
        return;

    bool transferRequired = false;

    AABB **reqZones = new AABB*[numberOfWorkers];
    AABB *reqZones_data = new AABB[numberOfWorkers*numberOfWorkers];
    for ( int i = 0; i < numberOfWorkers; ++i ) {
        reqZones[i] = reqZones_data + (i*numberOfWorkers);
    }
    auto& engine = Engine::getInstance();
    GCMDispatcher* dispatcher = engine.getDispatcher();

    // FIXME@avasyukov - workaround for SphProxyDispatcher
    // But we still need this
    if( dispatcher->getOutline(0) == NULL ) {
        THROW_BAD_METHOD("We can't do this because it will cause all MPI routines to freeze");
        return;
    }

    BARRIER("DataBus::syncMissedNodes#1");

    //Body* body = engine.getBodyById( engine.getDispatcher()->getMyBodyId() );
    //TetrMeshSecondOrder* mesh = (TetrMeshSecondOrder*)body->getMeshes();
    TetrMeshSecondOrder* mesh = (TetrMeshSecondOrder*) _mesh;
    AABB* areaOfInterest = &(mesh->areaOfInterest);
    if( (mesh->syncedArea).includes( areaOfInterest ) )
    {
        LOG_DEBUG("We need nothing");
    }
    else
    {
        LOG_DEBUG("Yes, we need additional nodes");
        for (int i = 0 ; i < numberOfWorkers; i++)
        {
            if( i != rank)
            {
                LOG_DEBUG("Our area of interest: " << *areaOfInterest << " Outline[" << i << "]: " << *(dispatcher->getOutline(i)));
                areaOfInterest->findIntersection(dispatcher->getOutline(i), &reqZones[rank][i]);
            }
        }
    }

    BARRIER("DataBus::syncMissedNodes#2");

    MPI::COMM_WORLD.Allgather(
        MPI_IN_PLACE,
        numberOfWorkers, MPI_OUTLINE,
        reqZones_data,
        numberOfWorkers, MPI_OUTLINE
    );

    BARRIER("DataBus::syncMissedNodes#3");

    vector<AABB> *_reqZones = new vector<AABB>[numberOfWorkers];
    for (int i = 0 ; i < numberOfWorkers; i++)
        for (int j = 0 ; j < numberOfWorkers; j++)
        {
            _reqZones[i].push_back(reqZones[i][j]);
            if( !std::isinf(reqZones[i][j].minX) )
            {
                transferRequired = true;
            }
        }

    if(transferRequired)
    {
        transferNodes(mesh, _reqZones);
        BARRIER("DataBus::syncMissedNodes#3");
        //LOG_DEBUG("Rebuilding data types");
        //createDynamicTypes();
        LOG_DEBUG("Processing mesh after the sync");
        // Overhead
        for( int z = 0; z < engine.getNumberOfBodies(); z++ )
        {
            TetrMeshSecondOrder* tmpMesh = (TetrMeshSecondOrder*) engine.getBody(z)->getMeshes();
            tmpMesh->preProcess();
            tmpMesh->checkTopology(tau);
        }
        //FIXME@avasyukov - rethink it
        for( int z = 0; z < 3; z++ )
        {
            (mesh->syncedArea).min_coords[z] = (mesh->areaOfInterest).min_coords[z] - EQUALITY_TOLERANCE;
            (mesh->syncedArea).max_coords[z] = (mesh->areaOfInterest).max_coords[z] + EQUALITY_TOLERANCE;
        }
    }

    for (int i = 0 ; i < numberOfWorkers; i++)
        _reqZones[i].clear();
    delete[] _reqZones;

    delete[] reqZones_data;
    delete[] reqZones;
}

void DataBus::transferNodes(TetrMeshSecondOrder* mesh, vector<AABB>* _reqZones)
{
    AABB **reqZones = new AABB*[numberOfWorkers];
    AABB *reqZones_data = new AABB[numberOfWorkers*numberOfWorkers];
    for ( int i = 0; i < numberOfWorkers; ++i ) {
        reqZones[i] = reqZones_data + (i*numberOfWorkers);
    }

    for (int i = 0 ; i < numberOfWorkers; i++)
        for (int j = 0 ; j < numberOfWorkers; j++)
            if( !std::isinf(_reqZones[i][j].minX) )
            {
                reqZones[i][j] = _reqZones[i].at(j);
                LOG_DEBUG("CPU " << i << " asks from CPU " << j << " area: " << reqZones[i][j]);
            }

    auto& engine = Engine::getInstance();
    Body* body = engine.getBodyById( engine.getDispatcher()->getMyBodyId() );
    TetrMeshSecondOrder* myMesh = (TetrMeshSecondOrder*)body->getMeshes();

    int numberOfNodes[numberOfWorkers][numberOfWorkers];
    int numberOfTetrs[numberOfWorkers][numberOfWorkers];
    for (int i = 0 ; i < numberOfWorkers; i++)
        for (int j = 0 ; j < numberOfWorkers; j++)
        {
            numberOfNodes[i][j] = 0;
            numberOfTetrs[i][j] = 0;
        }

    // Looking how many nodes and tetrs we are going to send
    map<int,int>* sendNodesMap = new map<int,int>[numberOfWorkers];
    map<int,int>* addNodesMap = new map<int,int>[numberOfWorkers];
    map<int,int>* sendTetrsMap = new map<int,int>[numberOfWorkers];

    for (int i = 0 ; i < numberOfWorkers; i++)
    {
        if( !std::isinf(reqZones[i][rank].minX) )
        {
            for( int j = 0; j < myMesh->nodesNumber; j++ )
            {
                CalcNode& node = myMesh->getNodeByLocalIndex(j);
                if( reqZones[i][rank].isInAABB( node ) )
                {
                    numberOfNodes[rank][i]++;
                    sendNodesMap[i][ node.number ] = j;
                }
            }
            for( int j = 0; j < myMesh->tetrsNumber; j++ )
            {
                TetrSecondOrder& tetr = myMesh->getTetr2ByLocalIndex(j);
                if( sendNodesMap[i].find(tetr.verts[0]) != sendNodesMap[i].end()
                    || sendNodesMap[i].find(tetr.verts[1]) != sendNodesMap[i].end()
                    || sendNodesMap[i].find(tetr.verts[2]) != sendNodesMap[i].end()
                    || sendNodesMap[i].find(tetr.verts[3]) != sendNodesMap[i].end()
                        || sendNodesMap[i].find(tetr.addVerts[0]) != sendNodesMap[i].end()
                        || sendNodesMap[i].find(tetr.addVerts[1]) != sendNodesMap[i].end()
                        || sendNodesMap[i].find(tetr.addVerts[2]) != sendNodesMap[i].end()
                        || sendNodesMap[i].find(tetr.addVerts[3]) != sendNodesMap[i].end()
                        || sendNodesMap[i].find(tetr.addVerts[4]) != sendNodesMap[i].end()
                        || sendNodesMap[i].find(tetr.addVerts[5]) != sendNodesMap[i].end() )
                {
                    numberOfTetrs[rank][i]++;
                    sendTetrsMap[i][ tetr.number ] = j;
                    for( int k = 0; k < 4; k++ )
                    {
                        if( sendNodesMap[i].find(tetr.verts[k]) == sendNodesMap[i].end()
                                && addNodesMap[i].find(tetr.verts[k]) == addNodesMap[i].end() )
                        {
                            numberOfNodes[rank][i]++;
                            addNodesMap[i][ tetr.verts[k] ] = myMesh->getNodeLocalIndex(tetr.verts[k]);
                        }
                    }
                    for( int k = 0; k < 6; k++ )
                    {
                        if( sendNodesMap[i].find(tetr.addVerts[k]) == sendNodesMap[i].end()
                                && addNodesMap[i].find(tetr.addVerts[k]) == addNodesMap[i].end() )
                        {
                            numberOfNodes[rank][i]++;
                            addNodesMap[i][ tetr.addVerts[k] ] = myMesh->getNodeLocalIndex(tetr.addVerts[k]);
                        }
                    }
                }
            }
        }
    }

    BARRIER("DataBus::transferNodes#1");
    MPI::COMM_WORLD.Allgather(
        MPI_IN_PLACE,
        numberOfWorkers, MPI_INT,
        numberOfNodes,
        numberOfWorkers, MPI_INT
    );

    BARRIER("DataBus::transferNodes#2");

    MPI::COMM_WORLD.Allgather(
        MPI_IN_PLACE,
        numberOfWorkers, MPI_INT,
        numberOfTetrs,
        numberOfWorkers, MPI_INT
    );

    BARRIER("DataBus::transferNodes#3");

    for (int i = 0 ; i < numberOfWorkers; i++)
        for (int j = 0 ; j < numberOfWorkers; j++)
            if( numberOfNodes[i][j] != 0 )
            {
                LOG_DEBUG("CPU " << i << " is going to send to CPU " << j << " "
                        << numberOfNodes[i][j] << " nodes and " << numberOfTetrs[i][j] << " tetrs");
//                if( rank == j && mesh->getNodesNumber() == 0 )
//                    mesh->createNodes(numberOfNodes[i][j]);
//                if( rank == j && mesh->getTetrsNumber() == 0 )
//                    mesh->createTetrs(numberOfTetrs[i][j]);
            }


    vector<MPI::Request> reqs;
    CalcNode** recNodes = new CalcNode*[numberOfWorkers];
    TetrSecondOrder** recTetrs = new TetrSecondOrder*[numberOfWorkers];
    for( int i = 0; i < numberOfWorkers; i++ )
    {
        if( i != rank && numberOfNodes[i][rank] > 0 )
        {
            recNodes[i] = new CalcNode[numberOfNodes[i][rank]];
            recTetrs[i] = new TetrSecondOrder[numberOfTetrs[i][rank]];
            reqs.push_back(
                MPI::COMM_WORLD.Irecv(
                    recNodes[i],
                    numberOfNodes[i][rank],
                    MPI_ELNODE_NUMBERED,
                    i,
                    TAG_GET_TETRS_N+i
                )
            );
            reqs.push_back(
                MPI::COMM_WORLD.Irecv(
                    recTetrs[i],
                    numberOfTetrs[i][rank],
                    MPI_TETR_NUMBERED,
                    i,
                    TAG_GET_TETRS_T+i
                )
            );
        }
        else
        {
            recNodes[i] = NULL;
            recTetrs[i] = NULL;
        }
    }

    int max_len = 0;
    for (int i = 0; i< numberOfWorkers; i++)
    {
        if (numberOfNodes[rank][i] > max_len)
            max_len = numberOfNodes[rank][i];
        if (numberOfTetrs[rank][i] > max_len)
            max_len = numberOfTetrs[rank][i];
    }
    int *lens = new int[max_len];
    for (int i = 0; i < max_len; i++)
        lens[i] = 1;

    MPI::Datatype *n = new MPI::Datatype[numberOfWorkers];
    MPI::Datatype *t = new MPI::Datatype[numberOfWorkers];
    vector<int> displ;
    map<int, int>::const_iterator itr;
    for( int i = 0; i < numberOfWorkers; i++ )
    {
        if( i != rank && numberOfNodes[rank][i] > 0 )
        {
            displ.clear();
            for( itr = sendNodesMap[i].begin(); itr != sendNodesMap[i].end(); ++itr )
                displ.push_back(itr->second);
            for( itr = addNodesMap[i].begin(); itr != addNodesMap[i].end(); ++itr )
                displ.push_back(itr->second);
            sort( displ.begin(), displ.end() );

            n[i] = MPI_ELNODE_NUMBERED.Create_indexed(numberOfNodes[rank][i], lens, &displ[0]);
            n[i].Commit();

            displ.clear();
            for( itr = sendTetrsMap[i].begin(); itr != sendTetrsMap[i].end(); ++itr )
                displ.push_back(itr->second);
            sort( displ.begin(), displ.end() );

            t[i] = MPI_TETR_NUMBERED.Create_indexed(numberOfTetrs[rank][i], lens, &displ[0]);
            t[i].Commit();

            reqs.push_back(
                MPI::COMM_WORLD.Isend(
                    &(myMesh->nodes[0]),
                    1,
                    n[i],
                    i,
                    TAG_GET_TETRS_N+rank
                )
            );
            reqs.push_back(
                MPI::COMM_WORLD.Isend(
                    &(myMesh->tetrs2[0]),//mesh->getTetrByLocalIndex(0),
                    1,
                    t[i],
                    i,
                    TAG_GET_TETRS_T+rank
                )
            );
        }
    }

    // FIXME - we suppose here that one process will send nodes for one mesh only (!)
    TetrMeshSecondOrder* targetMesh = NULL;
    MPI::Request::Waitall(reqs.size(), &reqs[0]);
    BARRIER("DataBus::transferNodes#4");
    LOG_DEBUG("Processing received data");
    for( int i = 0; i < numberOfWorkers; i++ )
    {
        if( i != rank && numberOfNodes[i][rank] > 0 )
        {
            LOG_DEBUG("Processing nodes");
            LOG_DEBUG("Worker " << rank << " data from " << i << ". "
                        << "Nodes size " << numberOfNodes[i][rank] << " "
                        << "Tetrs size " << numberOfTetrs[i][rank]);
            for( int j = 0; j < numberOfNodes[i][rank]; j++ )
            {
                int num = recNodes[i][j].number;
                unsigned char bodyNum = recNodes[i][j].bodyId;
                targetMesh = (TetrMeshSecondOrder*) engine.getBody(bodyNum)->getMeshes();
                if( targetMesh->getNodesNumber() == 0 )
                {
                    targetMesh->createNodes( numberOfNodes[i][rank] );
                    LOG_DEBUG("Nodes storage created for body " << (int)bodyNum << ". Size: " << numberOfNodes[i][rank]);
                }
                if( ! targetMesh->hasNode(num) )
                {
                    recNodes[i][j].setPlacement(false);
                    targetMesh->addNode(recNodes[i][j]);
                }
            }
            LOG_DEBUG("Processing tetrs");
            if( targetMesh->getTetrsNumber() == 0 )
            {
                targetMesh->createTetrs( numberOfTetrs[i][rank] );
                LOG_DEBUG("Tetrs storage created. Size: " << numberOfTetrs[i][rank]);
            }
            for( int j = 0; j < numberOfTetrs[i][rank]; j++ )
            {
                int num = recTetrs[i][j].number;
                if( ! targetMesh->hasTetr(num) )
                {
                    targetMesh->addTetr2(recTetrs[i][j]);
                }
            }
        }
    }
    reqs.clear();
    for( int i = 0; i < numberOfWorkers; i++ )
    {
        if( i != rank && numberOfNodes[rank][i] > 0 )
        {
            n[i].Free();
            t[i].Free();
        }
        if( recNodes[i] != NULL )
            delete[] recNodes[i];
        if( recTetrs[i] != NULL )
            delete[] recTetrs[i];
    }
    delete[] recNodes;
    delete[] recTetrs;
    delete[] sendNodesMap;
    delete[] addNodesMap;
    delete[] sendTetrsMap;
    delete[] lens;
    delete[] n;
    delete[] t;
    delete[] reqZones_data;
    delete[] reqZones;
    LOG_DEBUG("Nodes transfer done");
}
