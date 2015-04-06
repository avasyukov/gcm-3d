#include "libgcm/util/formats/Ani3DTetrFileReader.hpp"

#include "libgcm/util/AABB.hpp"
#include "libgcm/mesh/tetr/TetrMeshFirstOrder.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/GCMDispatcher.hpp"

using namespace gcm;
using std::string;
using std::vector;
using std::map;
using std::numeric_limits;
using std::ifstream;

Ani3DTetrFileReader::Ani3DTetrFileReader()
{
    INIT_LOGGER("gcm.Ani3DTetrFileReader");
}

Ani3DTetrFileReader::~Ani3DTetrFileReader()
{

}

void Ani3DTetrFileReader::preReadFile(string file, AABB* scene, int& sliceDirection, int& numberOfNodes)
{
    scene->minX = numeric_limits<float>::infinity();
    scene->minY = numeric_limits<float>::infinity();
    scene->minZ = numeric_limits<float>::infinity();
    scene->maxX = - numeric_limits<float>::infinity();
    scene->maxY = - numeric_limits<float>::infinity();
    scene->maxZ = - numeric_limits<float>::infinity();

    string str;
    int tmp_int;
    float tmp_float;
    int number_of_nodes;

    ifstream infile;
    infile.open(file.c_str(), ifstream::in);
    if(!infile.is_open())
        THROW_INVALID_INPUT( "Can not open ani3d:out file" );

    LOG_DEBUG("Reading ani3d:out file...");

    infile >> number_of_nodes;
    LOG_DEBUG("File contains " << number_of_nodes << " nodes");

    for(int i = 0; i < number_of_nodes; i++)
    {
        infile >> tmp_float;
        if( tmp_float < scene->minX )
            scene->minX = tmp_float;
        if( tmp_float > scene->maxX )
            scene->maxX = tmp_float;

        infile >> tmp_float;
        if( tmp_float < scene->minY )
            scene->minY = tmp_float;
        if( tmp_float > scene->maxY )
            scene->maxY = tmp_float;

        infile >> tmp_float;
        if( tmp_float < scene->minZ )
            scene->minZ = tmp_float;
        if( tmp_float > scene->maxZ )
            scene->maxZ = tmp_float;
    }
    LOG_DEBUG("Finished reading nodes");

    LOG_DEBUG("Nodes Ok");

    infile.close();

    // FIXME - rewrite it
    LOG_DEBUG("Determine slicing direction");
    ifstream tmpfile;
    tmpfile.open(file.c_str(), ifstream::in);

    tmpfile >> number_of_nodes;

    int numberOfSlices = 10;
    int distrX[10];
    int distrY[10];
    int distrZ[10];
    for( int i = 0; i < 10; i++ )
        distrX[i] = distrY[i] = distrZ[i] = 0;

    float x,y,z;
    float sliceX = (scene->maxX - scene->minX ) / numberOfSlices;
    float sliceY = (scene->maxY - scene->minY ) / numberOfSlices;
    float sliceZ = (scene->maxZ - scene->minZ ) / numberOfSlices;
    float minSlice = sliceX;
    if( minSlice > sliceY )
        minSlice = sliceY;
    if( minSlice > sliceZ )
        minSlice = sliceZ;

    for(int i = 0; i < number_of_nodes; i++)
    {
        tmpfile >> x;
        tmpfile >> y;
        tmpfile >> z;
        int xZoneNum = (int)( (x - scene->minX) / sliceX );
        int yZoneNum = (int)( (y - scene->minY) / sliceY );
        int zZoneNum = (int)( (z - scene->minZ) / sliceZ );
        distrX[xZoneNum]++;
        distrY[yZoneNum]++;
        distrZ[zZoneNum]++;
    }
    tmpfile.close();

    int minDistrX = distrX[0];
    int maxDistrX = distrX[0];
    int minDistrY = distrY[0];
    int maxDistrY = distrY[0];
    int minDistrZ = distrZ[0];
    int maxDistrZ = distrZ[0];
    for( int i = 0; i < 10; i++ )
    {
        if( distrX[i] > maxDistrX )
            maxDistrX = distrX[i];
        if( distrX[i] < minDistrX )
            minDistrX = distrX[i];

        if( distrY[i] > maxDistrY )
            maxDistrY = distrY[i];
        if( distrY[i] < minDistrY )
            minDistrY = distrY[i];

        if( distrZ[i] > maxDistrZ )
            maxDistrZ = distrZ[i];
        if( distrZ[i] < minDistrZ )
            minDistrZ = distrZ[i];
    }
    float qualityX = (float)minDistrX / (float)maxDistrX;
    float qualityY = (float)minDistrY / (float)maxDistrY;
    float qualityZ = (float)minDistrZ / (float)maxDistrZ;

    LOG_DEBUG("File successfylly pre-read.");

    float qx = qualityX * sliceX / minSlice;
    float qy = qualityY * sliceY / minSlice;
    float qz = qualityZ * sliceZ / minSlice;

    int dir = 0;
    if( qx > qy )
        if( qx > qz )
            dir = 0;
        else
            dir = 2;
    else
        if( qy > qz )
            dir = 1;
        else
            dir = 2;

    LOG_DEBUG("qualityX: " << qualityX << " qualityY: " << qualityY << " qualityZ: " << qualityZ);
    LOG_DEBUG("sliceX: " << sliceX / minSlice << " sliceY: " << sliceY / minSlice << " sliceZ: " << sliceZ / minSlice );
    LOG_DEBUG("slice direction: " << dir);

    sliceDirection = dir;
    numberOfNodes = number_of_nodes;
}

void Ani3DTetrFileReader::readFile(string file, TetrMeshFirstOrder* mesh, GCMDispatcher* dispatcher, int rank, bool ignoreDispatcher)
{
    assert_true(mesh);
    assert_true(dispatcher);
    int tetrsCount = 0;
    int fileVer;
    string str;
    int tmp_int;
    float tmp_float;
    int number_of_nodes;
    int number_of_elements;

    ifstream infile;
    infile.open(file.c_str(), ifstream::in);
    if(!infile.is_open())
        THROW_INVALID_INPUT( "Can not open ani3d:out file" );

    LOG_DEBUG("Reading ani3d:out file...");

    infile >> number_of_nodes;
    LOG_DEBUG("File contains " << number_of_nodes << " nodes");
    vector<CalcNode*>* nodes = new vector<CalcNode*>;

    for(int i = 0; i < number_of_nodes; i++)
    {
        float coords[3];
        infile >> coords[0] >> coords[1] >> coords[2];
        if( ignoreDispatcher || dispatcher->isMine( coords, mesh->getBody()->getId() ) )
        {
            CalcNode* node = new CalcNode();
            node->number = i;
            node->coords[0] = coords[0];
            node->coords[1] = coords[1];
            node->coords[2] = coords[2];
            node->setPlacement(true);
            nodes->push_back( node );
        }
    }

    LOG_DEBUG("Finished reading nodes");
    LOG_DEBUG("There are " << nodes->size() << " local nodes");


    mesh->createNodes( nodes->size() );
    for(unsigned int i = 0; i < nodes->size(); i++)
    {
        mesh->addNode( *nodes->at(i) );
    }
    nodes->clear();
    delete nodes;

    LOG_DEBUG("Nodes Ok");

    infile >> number_of_elements;
    LOG_DEBUG("File contains " << number_of_elements << " elements");

    vector<TetrFirstOrder*>* tetrs = new vector<TetrFirstOrder*>;

    for(int i = 0; i < number_of_elements; i++)
    {
        tetrsCount++;
        int number = tetrs->size();
        int vert[4];
        infile >> vert[0] >> vert[1] >> vert[2] >> vert[3] >> tmp_int;

        if( (vert[0] <= 0) || (vert[1] <= 0) || (vert[2] <= 0) || (vert[3] <= 0) )
            THROW_INVALID_INPUT("Wrong file format");

        vert[0]--; vert[1]--; vert[2]--; vert[3]--;

        if( mesh->hasNode(vert[0])
                || mesh->hasNode(vert[1])
                || mesh->hasNode(vert[2])
                || mesh->hasNode(vert[3]) )
            tetrs->push_back( new TetrFirstOrder( number, vert ) );
	else
	{
	    LOG_DEBUG("Unkndes: " << tetrsCount <<" v: " <<vert[0] <<" " <<vert[1] <<" " <<vert[2] <<" "  <<vert[3] 
		<<" "<<mesh->hasNode(vert[0])<<" "<<mesh->hasNode(vert[1])<<" "<<mesh->hasNode(vert[2])<<" "<<mesh->hasNode(vert[3]));
            THROW_INVALID_INPUT("Unknown nodes for tetraedres");
	}
    }

    LOG_DEBUG("File contains " << tetrsCount << " tetrs");
    LOG_DEBUG("There are " << tetrs->size() << " local tetrs");

    map<int,int> remoteNodes;
    mesh->createTetrs( tetrs->size() );
    for(unsigned int i = 0; i < tetrs->size(); i++)
    {
        TetrFirstOrder* tetr = tetrs->at(i);
        mesh->addTetr( *tetr );
        for(int j = 0; j < 4; j++)
            if( ! mesh->hasNode( tetr->verts[j] ) )
                remoteNodes[tetr->verts[j]] = i;
    }
    tetrs->clear();
    delete tetrs;

    LOG_DEBUG("Finished reading elements");

    LOG_DEBUG("Elements Ok");

    infile.close();

    LOG_DEBUG("Reading required remote nodes");
    LOG_DEBUG("We expect " << remoteNodes.size() << " nodes" );
    int remoteNodesCount = 0;

    infile.open(file.c_str(), ifstream::in);
    if(!infile.is_open())
        THROW_INVALID_INPUT( "Can not open ani3d:out file" );

    infile >> number_of_nodes;
    CalcNode tmpNode;
    for(int i = 0; i < number_of_nodes; i++)
    {
        infile >> tmpNode.coords[0] >> tmpNode.coords[1] >> tmpNode.coords[2];
        tmpNode.number = i;
        if( remoteNodes.find( tmpNode.number ) != remoteNodes.end() )
        {
            tmpNode.setPlacement(false);
            mesh->addNode(tmpNode);
            remoteNodesCount++;
        }
    }

    infile.close();

    LOG_DEBUG("Read " << remoteNodesCount << " remote nodes");

    LOG_DEBUG("Finished reading nodes");

    LOG_DEBUG("File successfylly read.");

    LOG_DEBUG("There are " << mesh->getNodesNumber() << " nodes is the mesh");
}
