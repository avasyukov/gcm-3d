#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <getopt.h>

#ifdef CONFIG_ENABLE_LOGGING
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/mdc.h>
#endif

#include <mpi.h>

#include "Engine.h"
#include "Utils.h"
#include "mesh/Mesh.h"
#include "Logging.h"

#include "xml.h"

#ifndef CONFIG_SHARE_GCM
#define CONFIG_SHARE_GCM "/usr/share/gcm3d"
#endif

using namespace std;
using namespace gcm;
using namespace xml;

/*
 * Returns value of named attribute.
 */

string getAttributeByName(AttrList attrs, string name) {
	AttrList::iterator iter = attrs.find(name);
	if (iter != attrs.end())
		return iter->second;
	THROW_INVALID_ARG("Attribute \"" + name + "\" not found in list");
}

void loadSceneFromFile(Engine& engine, string fileName)
{
	USE_LOGGER;
	INIT_LOGGER("gcm.launcher.TaskLoader");
	// FIXME shoul we validate task file against xml schema?
	FileLookupService& fls =  engine.getFileLookupService();
	string fname = fls.lookupFile(fileName);
	LOG_INFO("Loading scene from file " << fname);
	// parse file
	Doc doc = Doc(fname);
	xml::Node rootNode = doc.getRootElement();
	// read task parameters
	NodeList taskNodes = rootNode.xpath("/task");
	if( taskNodes.size() != 1 )
		THROW_INVALID_INPUT("Config file should contain one <task/> element");
	foreach(taskNode, taskNodes)
	{
		int numberOfSnaps = atoi( getAttributeByName(taskNode->getAttributes(), "numberOfSnaps").c_str() );
		int stepsPerSnap = atoi( getAttributeByName(taskNode->getAttributes(), "stepsPerSnap").c_str() );
		engine.setNumberOfSnaps(numberOfSnaps);
		engine.setStepsPerSnap(stepsPerSnap);
	}
	// reading materials
	NodeList matNodes = rootNode.xpath("/task/materials/material");
	foreach(matNode, matNodes)
	{
		string id = getAttributeByName(matNode->getAttributes(), "id");
		float la = atof( getAttributeByName(matNode->getAttributes(), "la").c_str() );
		float mu = atof( getAttributeByName(matNode->getAttributes(), "mu").c_str() );
		float rho = atof( getAttributeByName(matNode->getAttributes(), "rho").c_str() );
		if( la <= 0 || mu <= 0 || rho <= 0 )
			LOG_ERROR("Incorrect rheology in task file for material: " << id);
		LOG_DEBUG("Loaded material " << id << " with properties: (" << la << "; " << mu << "; " << rho << ")");
		Material* mat = new Material(id);
		mat->setRho(rho);
		mat->setLame(la, mu);
		engine.addMaterial(mat);
	}
	
	AABB globalScene;
	
	// search for bodies
	NodeList bodyNodes = rootNode.xpath("/task/bodies/body");
	
	// prepare basic bodies parameters
	foreach(bodyNode, bodyNodes)
	{
		string id = bodyNode->getAttributes()["id"];
		LOG_DEBUG("Loading body '" << id << "'");
		// create body instance
		Body* body = new Body(id);
		body->setEngine(engine);
		// set rheology
		NodeList rheologyNodes = bodyNode->getChildrenByName("rheology");
		if (rheologyNodes.size() > 1)
			THROW_INVALID_INPUT("Only one rheology element allowed for body declaration");
		if (rheologyNodes.size()) {
			// We can do smth here when we have more than one rheology calculators
		}
		
		// preload meshes for dispatcher
		NodeList meshNodes = bodyNode->getChildrenByName("mesh");
		foreach(meshNode, meshNodes)
		{
			Params params = Params(meshNode->getAttributes());
			if (!params.has("type"))
				THROW_INVALID_INPUT("Mesh type is not specified.");
			
			MeshLoader* meshLoader = engine.getMeshLoader(params["type"]);
			if (!meshLoader)
				THROW_INVALID_INPUT("Mesh loader not found.");
			
			LOG_INFO("Preloading mesh");
			
			AABB localScene;
			meshLoader->preLoadMesh(params, &localScene);
			
			// transform meshes
			NodeList transformNodes = bodyNode->getChildrenByName("transform");
			foreach(transformNode, transformNodes)
			{
				string transformType = getAttributeByName(transformNode->getAttributes(), "type");
				if( transformType == "translate" )
				{
					float x = atof( getAttributeByName(transformNode->getAttributes(), "moveX").c_str() );
					float y = atof( getAttributeByName(transformNode->getAttributes(), "moveY").c_str() );
					float z = atof( getAttributeByName(transformNode->getAttributes(), "moveZ").c_str() );
					LOG_DEBUG("Moving body: [" << x << "; " << y << "; " << z << "]");
					localScene.transfer(x, y, z);
				}
			}
			LOG_DEBUG("Mesh preloaded. Mesh size: " << localScene );
			
			engine.getDispatcher()->addBodyOutline(id, localScene);
			
			if( isinf(globalScene.maxX) )
			{
				globalScene = localScene;
			}
			else
			{
				for( int k = 0; k < 3; k++ )
				{
					if( globalScene.min_coords[k] > localScene.min_coords[k] )
						globalScene.min_coords[k] = localScene.min_coords[k];
					if( globalScene.max_coords[k] < localScene.max_coords[k] )
						globalScene.max_coords[k] = localScene.max_coords[k];
				}
			}
		}
		
		// add body to scene
		engine.addBody(body);
	}
	
	engine.setScene(globalScene);
	LOG_DEBUG("Total scene: " << engine.getScene());
	
	// run dispatcher
	engine.getDispatcher()->prepare(engine.getNumberOfWorkers(), &globalScene);
	engine.getDataBus()->syncOutlines();
	for( int i = 0; i < engine.getNumberOfWorkers(); i++)
	{
		LOG_DEBUG("Area scheduled for worker " << i << ": " << *(engine.getDispatcher()->getOutline(i)));
	}	
	
	// read meshes for all bodies
	foreach(bodyNode, bodyNodes)
	{
		string id = bodyNode->getAttributes()["id"];
		LOG_DEBUG("Loading meshes for body '" << id << "'");
		// get body instance
		Body* body = engine.getBodyById(id);
		
		// FIXME - WA - we need this to determine isMine() correctly for moved points
		float dX = 0;
		float dY = 0;
		float dZ = 0;
		NodeList tmpTransformNodes = bodyNode->getChildrenByName("transform");
		foreach(transformNode, tmpTransformNodes)
		{
			string transformType = getAttributeByName(transformNode->getAttributes(), "type");
			if( transformType == "translate" )
			{
				dX += atof( getAttributeByName(transformNode->getAttributes(), "moveX").c_str() );
				dY += atof( getAttributeByName(transformNode->getAttributes(), "moveY").c_str() );
				dZ += atof( getAttributeByName(transformNode->getAttributes(), "moveZ").c_str() );
			}
		}
		if( engine.getNumberOfWorkers() != 1 )
			engine.getDispatcher()->setTransferVector(dX, dY, dZ, id);
		
		// load meshes
		NodeList meshNodes = bodyNode->getChildrenByName("mesh");
		foreach(meshNode, meshNodes)
		{
			Params params = Params(meshNode->getAttributes());
			MeshLoader* meshLoader = engine.getMeshLoader(params["type"]);
			
			LOG_INFO("Loading mesh");
			// use loader to load mesh
			Mesh* mesh = meshLoader->load(body, params);
			
			// attach mesh to body
			body->attachMesh(mesh);
			LOG_INFO("Mesh '" << mesh->getId() << "' of type '" <<  meshLoader->getType() << "' created");
		}
		
		// transform meshes
		NodeList transformNodes = bodyNode->getChildrenByName("transform");
		foreach(transformNode, transformNodes)
		{
			string transformType = getAttributeByName(transformNode->getAttributes(), "type");
			if( transformType == "translate" )
			{
				float x = atof( getAttributeByName(transformNode->getAttributes(), "moveX").c_str() );
				float y = atof( getAttributeByName(transformNode->getAttributes(), "moveY").c_str() );
				float z = atof( getAttributeByName(transformNode->getAttributes(), "moveZ").c_str() );
				LOG_DEBUG("Moving body: [" << x << "; " << y << "; " << z << "]");
				body->getMeshes()->transfer(x, y, z);
			}
		}
		
		// FIXME - Part of the WA above
		if( engine.getNumberOfWorkers() != 1 )
			engine.getDispatcher()->setTransferVector(-dX, -dY, -dZ, id);
		
		// set material properties
		NodeList matNodes = bodyNode->getChildrenByName("material");
		if (matNodes.size() < 1)
			THROW_INVALID_INPUT("Material not set");
		foreach(matNode, matNodes)
		{
			string id = getAttributeByName(matNode->getAttributes(), "id");
			Material* mat = engine.getMaterial(id);
			Mesh* mesh = body->getMeshes();
			
			NodeList areaNodes = matNode->getChildrenByName("area");
			if (areaNodes.size() == 0)
			{
				mesh->setRheology( engine.getMaterialIndex(id) );
			}
			else if (areaNodes.size() == 1)
			{
				Area* matArea = NULL;
				string areaType = getAttributeByName(areaNodes.front().getAttributes(), "type");
				if( areaType == "box" )
				{
					LOG_DEBUG("Material area: " << areaType);
					float minX = atof( getAttributeByName(areaNodes.front().getAttributes(), "minX").c_str() );
					float maxX = atof( getAttributeByName(areaNodes.front().getAttributes(), "maxX").c_str() );
					float minY = atof( getAttributeByName(areaNodes.front().getAttributes(), "minY").c_str() );
					float maxY = atof( getAttributeByName(areaNodes.front().getAttributes(), "maxY").c_str() );
					float minZ = atof( getAttributeByName(areaNodes.front().getAttributes(), "minZ").c_str() );
					float maxZ = atof( getAttributeByName(areaNodes.front().getAttributes(), "maxZ").c_str() );
					LOG_DEBUG("Box size: [" << minX << ", " << maxX << "] " 
										<< "[" << minY << ", " << maxY << "] " 
										<< "[" << minZ << ", " << maxZ << "]");
					matArea = new BoxArea(minX, maxX, minY, maxY, minZ, maxZ);
				} else {
					LOG_WARN("Unknown initial state area: " << areaType);
				}
				mesh->setRheology( engine.getMaterialIndex(id), matArea );
			}
			else
			{
				THROW_INVALID_INPUT("Only one or zero area elements are allowed for material");
			}
		}
		LOG_DEBUG("Body '" << id << "' loaded");
	}	
	
	// FIXME - rewrite this indian style code
	NodeList initialStateNodes = rootNode.xpath("/task/initialState");
	foreach(initialStateNode, initialStateNodes)
	{
		Area* stateArea = NULL;
		float values[9];
		NodeList areaNodes = initialStateNode->getChildrenByName("area");
		if (areaNodes.size() > 1)
			THROW_INVALID_INPUT("Only one area element allowed for initial state");
		if (areaNodes.size()) {
			string areaType = getAttributeByName(areaNodes.front().getAttributes(), "type");
			if( areaType == "box" )
			{
				LOG_DEBUG("Initial state area: " << areaType);
				float minX = atof( getAttributeByName(areaNodes.front().getAttributes(), "minX").c_str() );
				float maxX = atof( getAttributeByName(areaNodes.front().getAttributes(), "maxX").c_str() );
				float minY = atof( getAttributeByName(areaNodes.front().getAttributes(), "minY").c_str() );
				float maxY = atof( getAttributeByName(areaNodes.front().getAttributes(), "maxY").c_str() );
				float minZ = atof( getAttributeByName(areaNodes.front().getAttributes(), "minZ").c_str() );
				float maxZ = atof( getAttributeByName(areaNodes.front().getAttributes(), "maxZ").c_str() );
				LOG_DEBUG("Box size: [" << minX << ", " << maxX << "] " 
									<< "[" << minY << ", " << maxY << "] " 
									<< "[" << minZ << ", " << maxZ << "]");
				stateArea = new BoxArea(minX, maxX, minY, maxY, minZ, maxZ);
			} else {
				LOG_WARN("Unknown initial state area: " << areaType);
			}
		}
		NodeList valuesNodes = initialStateNode->getChildrenByName("values");
		if (valuesNodes.size() > 1)
			THROW_INVALID_INPUT("Only one values element allowed for initial state");
		if (valuesNodes.size()) {
			memset(values, 0, 9*sizeof(float));
			string vx = valuesNodes.front().getAttributes()["vx"];
			if( !vx.empty() )
				values[0] = atof( vx.c_str() );
			string vy = valuesNodes.front().getAttributes()["vy"];
			if( !vy.empty() )
				values[1] = atof( vy.c_str() );
			string vz = valuesNodes.front().getAttributes()["vz"];
			if( !vz.empty() )
				values[2] = atof( vz.c_str() );
			string sxx = valuesNodes.front().getAttributes()["sxx"];
			if( !sxx.empty() )
				values[3] = atof( sxx.c_str() );
			string sxy = valuesNodes.front().getAttributes()["sxy"];
			if( !sxy.empty() )
				values[4] = atof( sxy.c_str() );
			string sxz = valuesNodes.front().getAttributes()["sxz"];
			if( !sxz.empty() )
				values[5] = atof( sxz.c_str() );
			string syy = valuesNodes.front().getAttributes()["syy"];
			if( !syy.empty() )
				values[6] = atof( syy.c_str() );
			string syz = valuesNodes.front().getAttributes()["syz"];
			if( !syz.empty() )
				values[7] = atof( syz.c_str() );
			string szz = valuesNodes.front().getAttributes()["szz"];
			if( !szz.empty() )
				values[8] = atof( szz.c_str() );
			LOG_DEBUG("Initial state values: " 
							<< values[0] << " " << values[1] << " " << values[2] << " " 
							<< values[3] << " " << values[4] << " " << values[5] << " "
							<< values[6] << " " << values[7] << " " << values[8] );
		}
		for( int i = 0; i < engine.getNumberOfBodies(); i++ )
			engine.getBody(i)->setInitialState(stateArea, values);
	}
	LOG_DEBUG("Scene loaded");
}

// This function print usage message
void print_help(char* binaryName)
{
	cout << "\nUsage: " << binaryName << " --task file --data-dir dir\n" 
		<< "\t--task - xml file with task description\n"
		<< "\t--data-dir - directory with models specified in task\n";
};

int main(int argc, char **argv, char **envp)
{
	FileLookupService fls;
	
	// Parse command line options
	int c;
	static struct option long_options[] =
	{
		{"task"      , required_argument, 0, 't'},
		{"data-dir"  , required_argument, 0, 'd'},
		{"help"      , no_argument      , 0, 'h'},
		{0           , 0                , 0, 0  }
	};
	int option_index = 0;
	
	string taskFile;
	string dataDir;
	
	while ((c = getopt_long (argc, argv, "t:d:h:", long_options, &option_index)) != -1)
	{
		switch (c)
		{
			case 't':
				taskFile = optarg;
				break;
			case 'd':
				dataDir = optarg;
				if(dataDir[dataDir.length()-1] == '/')
					dataDir[dataDir.length()-1] = '\0';
				break;
			case 'h':
				print_help(argv[0]);
				return 0;
			case '?':
				print_help(argv[0]);
			default:
				return -1;
		}
	}

	USE_AND_INIT_LOGGER("gcm");

	try {
		fls.addPath(CONFIG_SHARE_GCM);
		fls.addPath("./src/launcher/");

		#ifdef CONFIG_ENABLE_LOGGING
		MPI::Init();
		char pe[5];
		sprintf(pe, "%d", MPI::COMM_WORLD.Get_rank());
		log4cxx::MDC::put("PE", pe);
		log4cxx::PropertyConfigurator::configure(fls.lookupFile("log4cxx.properties"));
		#endif

		if( taskFile.empty() )
			THROW_INVALID_ARG("No task file provided");
		if( dataDir.empty() )
			dataDir = CONFIG_SHARE_GCM;
		LOG_INFO("Staring with taskFile '" << taskFile << "' and dataDir '" << dataDir << "'");
		
		Engine& engine = Engine::getInstance();
		engine.getFileLookupService().addPath(dataDir);
		loadSceneFromFile(engine, taskFile);
		engine.calculate();
		
	} catch (Exception &e) {
		LOG_FATAL("Exception was thrown: " << e.getMessage() << "\n @" << e.getFile() << ":" << e.getLine() << "\nCall stack: \n"<< e.getCallStack());
	}
}	
