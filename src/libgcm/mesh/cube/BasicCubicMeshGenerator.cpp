#include "mesh/cube/BasicCubicMeshGenerator.h"

#include "node/CalcNode.h"

const string gcm::BasicCubicMeshGenerator::PARAM_H          = "h";
const string gcm::BasicCubicMeshGenerator::PARAM_NUMBER     = "num";

string gcm::BasicCubicMeshGenerator::getType(){
	return "cube";
}

gcm::BasicCubicMeshGenerator::BasicCubicMeshGenerator() {
	INIT_LOGGER("gcm.BasicCubicMeshGenerator");
}

gcm::BasicCubicMeshGenerator::~BasicCubicMeshGenerator() {
}

void gcm::BasicCubicMeshGenerator::checkParams(Params params)
{
	if (params.find(PARAM_H) == params.end()) {
		THROW_INVALID_ARG("Cube size was not provided");
	}
	if (params.find(PARAM_NUMBER) == params.end()) {
		THROW_INVALID_ARG("Number of cubes was not provided");
	}
	
	if( engine->getNumberOfWorkers() > 1 )
		THROW_UNSUPPORTED("Cubic mesh can not be used in parallel calculations now. Sorry. Please run with -np 1.");
	if( engine->getNumberOfBodies() > 1 )
		THROW_UNSUPPORTED("Cubic mesh supports only single body tasks now. Sorry. Please fix task file.");
}

void gcm::BasicCubicMeshGenerator::loadMesh(Params params, BasicCubicMesh* mesh, GCMDispatcher* dispatcher)
{
	checkParams(params);
	float h = atof(params[PARAM_H].c_str());
	float num = atoi(params[PARAM_NUMBER].c_str());
	for( int k = 0; k <= num; k++ )
		for( int j = 0; j <= num; j++ )
			for( int i = 0; i <= num; i++ )
			{
				int n = i*(num+1)*(num+1) + j*(num+1) + k;
				float x = i*h;
				float y = j*h;
				float z = k*h;
				CalcNode* node = new CalcNode();//(n, x, y, z);
				node->number = n;
				node->coords[0] = x;
				node->coords[1] = y;
				node->coords[2] = z;
				node->setPlacement(Local);
				mesh->addNode( *node );
			}
	mesh->preProcess();
}

void gcm::BasicCubicMeshGenerator::preLoadMesh(Params params, AABB* scene, int& sliceDirection, int& numberOfNodes)
{
	checkParams(params);
	float h = atof(params[PARAM_H].c_str());
	float num = atoi(params[PARAM_NUMBER].c_str());
	sliceDirection = 0;
	numberOfNodes = (num + 1) * (num + 1) * (num + 1);
	scene->minX = scene->minY = scene->minZ = 0;
	scene->maxX = scene->maxY = scene->maxZ = num * h;
}
