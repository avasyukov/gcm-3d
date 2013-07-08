#include <iostream>
#include <string>
#include <vector>
#include <exception>

#ifdef CONFIG_ENABLE_LOGGING
#include <log4cxx/propertyconfigurator.h>
#endif

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

void loadSceneFromFile(Engine *engine, string fileName)
{
	USE_LOGGER;
	INIT_LOGGER("gcm.launcher.TaskLoader");
	// FIXME shoul we validate task file against xml schema?
	FileLookupService& fls =  engine->getFileLookupService();
	string fname = fls.lookupFile(fileName);
	LOG_INFO("Loading scene from file " << fname);
	// parse file
	Doc doc = Doc(fname);
	xml::Node rootNode = doc.getRootElement();
	// search for bodies
	NodeList bodyNodes = rootNode.xpath("/task/bodies/body");
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
			body->setRheology(getAttributeByName(rheologyNodes.front().getAttributes(), "type"));
		}

		// load meshes
		NodeList meshNodes = bodyNode->getChildrenByName("mesh");
		foreach(meshNode, meshNodes)
		{
			Params params = Params(meshNode->getAttributes());
			if (!params.has("type"))
			{
				// FIXME should we throw exception here?
				LOG_WARN("Mesh type is not specified, mesh will be ignored.");
				break;
			}
			MeshLoader* meshLoader = engine->getMeshLoader(params["type"]);
			if (!meshLoader)
			{
				// FIXME should we throw exception?
				LOG_WARN("Mesh loader for type '" << params["type"] << "' " <<
						"not found, mesh will be ignored.");
				break;
			}
			
			LOG_INFO("Loading mesh");
			// TODO - think about multiple bodies and multiple meshes per body
			AABB scene;
			meshLoader->preLoadMesh(params, &scene);
			engine->setScene(scene);
			LOG_DEBUG("Mesh preloaded. Scene size: " << engine->getScene() );

			engine->getDispatcher()->prepare(engine->getNumberOfWorkers(), &scene);
			engine->getDataBus()->syncOutlines();
			for( int i = 0; i < engine->getNumberOfWorkers(); i++)
			{
				LOG_DEBUG("Area scheduled for worker " << i << ": " << *(engine->getDispatcher()->getOutline(i)));
			}	
			
			// use loader to load mesh
			Mesh* mesh = meshLoader->load(body, params);
			// attach mesh to body
			body->attachMesh(mesh);
			LOG_INFO("Mesh '" << mesh->getId() << "' of type '" <<  meshLoader->getType() << "' created");
		}
		// add body to scene
		engine->addBody(body);
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
		for( int i = 0; i < engine->getNumberOfBodies(); i++ )
			engine->getBody(i)->setInitialState(stateArea, values);
	}
	LOG_DEBUG("Scene loaded");
}

int main(int argc, char **argv, char **envp)
{
	Engine* engine = new Engine();

	FileLookupService& fls =  engine->getFileLookupService();
	fls.addPath(CONFIG_SHARE_GCM);

	#ifdef CONFIG_ENABLE_LOGGING
	log4cxx::PropertyConfigurator::configure(fls.lookupFile("log4cxx.properties"));
	#endif

	USE_AND_INIT_LOGGER("gcm");


	try {
		loadSceneFromFile(engine, "tasks/test.xml");
		
		TetrMeshSecondOrder* mesh = (TetrMeshSecondOrder*)engine->getBody(0)->getMeshes();
		/*AABB* zone1 = new AABB(0, 70, 0, 19.1, 0, 70);
		AABB* zone2 = new AABB(0, 70, 19.2, 39, 0, 70);
		mesh->setRheology(53, 268, 1, zone2);
		mesh->setRheology(53, 268, 4.94, zone1);*/
		mesh->setRheology(70000, 10000, 1);
		
		engine->setNumberOfSnaps(10);
		engine->setStepsPerSnap(1);
		engine->calculate();
		
		//delete zone1;
		//delete zone2;
		delete engine;
	} catch (Exception &e) {
		LOG_FATAL("Exception was thrown: " << e.getMessage() << "\n @" << e.getFile() << ":" << e.getLine() << "\nCall stack: \n"<< e.getCallStack());
	}
}	
