#include "libgcm/util/formats/AniVtuTetrFileReader.hpp"

#include "libgcm/util/AABB.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/GCMDispatcher.hpp"

#include <unordered_map>

using namespace gcm;
using std::string;
using std::map;
using std::vector;
using std::numeric_limits;
using std::unordered_map;

AniVtuTetrFileReader::AniVtuTetrFileReader()
{
    INIT_LOGGER("gcm.AniVtuTetrFileReader");
}

AniVtuTetrFileReader::~AniVtuTetrFileReader()
{

}

void AniVtuTetrFileReader::preReadFile(string file, AABB* scene, int& sliceDirection, int& numberOfNodes)
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
    LOG_INFO("Number of points: " << g->GetNumberOfPoints());
    LOG_INFO("Number of cells: " << g->GetNumberOfCells());

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
    LOG_INFO("File successfylly pre-read.");
    // z-axis
    sliceDirection = 2;
    numberOfNodes = g->GetNumberOfPoints();
}

void AniVtuTetrFileReader::readFile(string file, TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher, int rank)
{
    readFile(file, mesh, dispatcher, rank, false);
}

void AniVtuTetrFileReader::readFile(string file, TetrMeshSecondOrder* soMesh, GCMDispatcher* dispatcher, int rank, bool ignoreDispatcher)
{
    TetrMeshSecondOrder* mesh = new TetrMeshSecondOrder();
    
    vtkXMLUnstructuredGridReader *xgr = vtkXMLUnstructuredGridReader::New();
    vtkUnstructuredGrid *g = vtkUnstructuredGrid::New();

    xgr->SetFileName(const_cast<char*>(file.c_str()));
    xgr->Update();

    g = xgr->GetOutput();
    if( ignoreDispatcher )
    {
        LOG_INFO("Reading file ignoring dispatcher");
    }
    else
    {
        LOG_INFO("Dispatcher zones:");
        dispatcher->printZones();
    }
    LOG_INFO("Number of points: " << g->GetNumberOfPoints());
    LOG_INFO("Number of cells: " << g->GetNumberOfCells());

    double v[3];
    /*vtkDoubleArray *vel = (vtkDoubleArray*) g->GetPointData()->GetArray("velocity");
    vel->SetNumberOfComponents(3);
    vtkDoubleArray *sxx = (vtkDoubleArray*) g->GetPointData()->GetArray("sxx");
    vtkDoubleArray *sxy = (vtkDoubleArray*) g->GetPointData()->GetArray("sxy");
    vtkDoubleArray *sxz = (vtkDoubleArray*) g->GetPointData()->GetArray("sxz");
    vtkDoubleArray *syy = (vtkDoubleArray*) g->GetPointData()->GetArray("syy");
    vtkDoubleArray *syz = (vtkDoubleArray*) g->GetPointData()->GetArray("syz");
    vtkDoubleArray *szz = (vtkDoubleArray*) g->GetPointData()->GetArray("szz");*/
    vtkIntArray *matId = (vtkIntArray*) g->GetPointData()->GetArray("materialID");
    /*vtkDoubleArray *rho = (vtkDoubleArray*) g->GetPointData()->GetArray("rho");*/
    vtkIntArray *nodeNumber = (vtkIntArray*) g->GetPointData ()->GetArray("nodeNumber");
    /*vtkIntArray *publicFlags = (vtkIntArray*) g->GetPointData ()->GetArray("publicFlags");
    vtkIntArray *privateFlags = (vtkIntArray*) g->GetPointData ()->GetArray("privateFlags");
    vtkIntArray *nodeBorderConditionId = (vtkIntArray*) g->GetPointData ()->GetArray("borderConditionId");*/

    unordered_map<uint, uint> ind;
    
    vector<CalcNode*>* nodes = new vector<CalcNode*>();
    nodes->reserve(g->GetNumberOfPoints());
    for( int i = 0; i < g->GetNumberOfPoints(); i++ )
    {
        double* dp = g->GetPoint(i);
        if( ignoreDispatcher || dispatcher->isMine( dp, soMesh->getBody()->getId() ) )
        {
            CalcNode* node = new CalcNode();
            node->number = nodeNumber->GetValue(i);
            node->coords[0] = dp[0];
            node->coords[1] = dp[1];
            node->coords[2] = dp[2];
            //vel->GetTupleValue(i, v);
            /*node->vx = 0;//v[0];
            node->vy = 0;//v[1];
            node->vz = 0;//v[2];
            node->sxx = 0;//sxx->GetValue(i);
            node->sxy = 0;//sxy->GetValue(i);
            node->sxz = 0;//sxz->GetValue(i);
            node->syy = 0;//syy->GetValue(i);
            node->syz = 0;//syz->GetValue(i);
            node->szz = 0;//szz->GetValue(i);*/
            node->setMaterialId( matId->GetValue(i) );
            //node->setRho( 0/*rho->GetValue(i)*/ );
            //node->setPublicFlags( 0/*publicFlags->GetValue(i)*/ );
            //node->setPrivateFlags( 0/*privateFlags->GetValue(i)*/ );
            //node->setBorderConditionId( 0/*nodeBorderConditionId->GetValue(i)*/ );
            node->setPlacement(true);
            //if( !ignoreDispatcher )
            //    node->setPlacement(true);
            nodes->push_back(node);
            ind[node->number] = nodes->size() - 1;
            /*if(node->number < 100) {
                LOG_INFO("Ani node: " << *node);
                LOG_INFO("Ind: " << ind[node->number]);
            }*/
        }
    }
    LOG_INFO("Finished reading nodes");
    LOG_INFO("There are " << nodes->size() << " local nodes");

    mesh->createNodes( nodes->size() );
    for(unsigned int i = 0; i < nodes->size(); i++)
    {
        mesh->addNode( *nodes->at(i) );
    }
    for(unsigned int i = 0; i < nodes->size(); i++)
    {
        delete( nodes->at(i));
    }
    nodes->clear();
    delete nodes;

    vtkIntArray* tetr1stOrderNodes = (vtkIntArray*) g->GetCellData ()->GetArray ("tetr1stOrderNodes");
    assert_eq(tetr1stOrderNodes->GetNumberOfComponents (), 4);

    vector<TetrFirstOrder*>* tetrs = new vector<TetrFirstOrder*>;

    TetrFirstOrder new_tetr;
    LOG_INFO("File contains " << g->GetNumberOfCells() << " tetrs");
    for( int i = 0; i < g->GetNumberOfCells(); i++ )
    {
        
        new_tetr.number = i;//tetrNumber->GetValue(i);
        //vtkCell* cell = g->GetCell(i);
        //assert_eq(cell->PointIds->GetNumberOfIds(), 4);
        tetr1stOrderNodes->GetTupleValue (i, new_tetr.verts);
        //for(int j = 0; j < cell->PointIds->GetNumberOfIds(); j++)
        //{
            //int pointId = cell->PointIds->GetId(j);
            //int nodeNum = nodeNumber->GetValue(pointId);
            //new_tetr.verts[j] = pointId - 1; //ind[nodeNum];
        /*for(int j =0; j < 4; j++) {
            if(i < 6) {
                LOG_INFO("Tetr: " << i);
                LOG_INFO("Vert: " << j);
                LOG_INFO("Res: " << new_tetr.verts[j]);
            }
        }*/
        //}
        //tetr1stOrderNodes->GetTupleValue (i, new_tetr.verts);
        //tetr2ndOrderNodes->GetTupleValue (i, new_tetr.addVerts);

        /*vtkTetra *vt = (vtkTetra*) g->GetCell(i);

        int vert[4];
        vert[0] = vt->GetPointId(0);
        vert[1] = vt->GetPointId(1);
        vert[2] = vt->GetPointId(2);
        vert[3] = vt->GetPointId(3);*/

        /*if( mesh->hasNode(new_tetr.verts[0])
                    || mesh->hasNode(new_tetr.verts[1])
                    || mesh->hasNode(new_tetr.verts[2])
                    || mesh->hasNode(new_tetr.verts[3]) )*/
        tetrs->push_back( new TetrFirstOrder( new_tetr.number, new_tetr.verts ) );
    }

    LOG_INFO("There are " << tetrs->size() << " local tetrs");

    //map<int,int> remoteNodes;
    mesh->createTetrs( tetrs->size() );
    for(unsigned int i = 0; i < tetrs->size(); i++)
    {
        TetrFirstOrder* tetr = tetrs->at(i);
        tetr->number = i;
        mesh->addTetr( *tetr );
        /*if(tetr->number < 6) {
            LOG_INFO("Tetr: " << *tetr);
            LOG_INFO("V1: " << tetr->verts[0]);
            LOG_INFO("V1: " << mesh->getNode(tetr->verts[0]));
            LOG_INFO("V2: " << tetr->verts[1]);
            LOG_INFO("V2: " << mesh->getNode(tetr->verts[1]));
            LOG_INFO("V3: " << tetr->verts[2]);
            LOG_INFO("V3: " << mesh->getNode(tetr->verts[2]));
            LOG_INFO("V4: " << tetr->verts[3]);
            LOG_INFO("V4: " << mesh->getNode(tetr->verts[3]));
        }*/
        /*for(int j = 0; j < 4; j++)
            if( ! mesh->hasNode( tetr->verts[j] ) )
                remoteNodes[tetr->verts[j]] = i;
        for(int j = 0; j < 6; j++)
            if( ! mesh->hasNode( tetr->addVerts[j] ) )
                remoteNodes[tetr->addVerts[j]] = i;*/
    }
    for(unsigned int i = 0; i < tetrs->size(); i++)
    {
        delete( tetrs->at(i) );
    }
    tetrs->clear();
    delete tetrs;

    LOG_INFO("Finished reading elements");

    LOG_INFO("File successfylly read.");
    
    soMesh->copyMesh(mesh);
}
