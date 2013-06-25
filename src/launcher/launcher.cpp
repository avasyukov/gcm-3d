#include <iostream>
#include <string>
#include <log4cxx/basicconfigurator.h>

#include "Engine.h"
#include "mesh/Mesh.h"
#include "Logging.h"

#include "libxml++/libxml++.h"

using namespace std;
using namespace gcm;
using namespace xmlpp;

/*
 * Returns value of named attribute.
 */

string getAttributeByName(xmlpp::Element::AttributeList attrs, string name) {
	foreach(attr, attrs)
		if ((*attr)->get_name() == name)
			return (*attr)->get_value();
	THROW_INVALID_ARG("Attribute \"" + name + "\" not found in list");
}

Params paramsFromXmlElement(xmlpp::Element* el) {
	xmlpp::Element::AttributeList attrs = el->get_attributes();
	Params result;
	foreach(attr, attrs)
	{
		result[(*attr)->get_name()] = (*attr)->get_value();
	}

	return result;
}


void loadSceneFromFile(Engine *engine, string fileName)
{
	USE_LOGGER;
	INIT_LOGGER("gcm.launcher.TaskLoader");
	// FIXME shoul we validate task file against xml schema?
	LOG_INFO("Loading scene from file " << fileName);
	// parse file
	xmlpp::DomParser parser;
	parser.parse_file(fileName);
	Element* rootElement = parser.get_document()->get_root_node();
	// search for bodies
	NodeSet bodyNodes = rootElement->find("/task/bodies/body");
	foreach(bodyNode, bodyNodes)
	{
		Element* bodyEl = dynamic_cast<Element*>(*bodyNode);
		string id = bodyEl->get_attribute_value("id");
		LOG_DEBUG("Loading body '" << id << "'");
		// create body instance
		Body* body = new Body(id);
		body->setEngine(engine);
		// set rheology
		xmlpp::Node::NodeList rheologyNodes = bodyEl->get_children("rheology");
		if (rheologyNodes.size() > 1)
			THROW_INVALID_INPUT("Only one rheology element allowed for body declaration");
		if (rheologyNodes.size()) {
			Element *rheologyEl = dynamic_cast<Element*>(rheologyNodes.front());
			body->setRheology(getAttributeByName(rheologyEl->get_attributes(), "type"));
		}

		// load meshes
		xmlpp::Node::NodeList meshNodes = bodyEl->get_children("mesh");
		foreach(meshNode, meshNodes)
		{
			Element* meshEl = static_cast<Element*>(*meshNode);
			// get mesh parameters
			Params params = paramsFromXmlElement(meshEl);
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
	NodeSet initialStateNodes = rootElement->find("/task/initialState");
	foreach(initialStateNode, initialStateNodes)
	{
		Element* stateEl = dynamic_cast<Element*>(*initialStateNode);
		Area* stateArea = NULL;
		float values[9];
		xmlpp::Node::NodeList areaNodes = stateEl->get_children("area");
		if (areaNodes.size() > 1)
			THROW_INVALID_INPUT("Only one area element allowed for initial state");
		if (areaNodes.size()) {
			Element *areaEl = dynamic_cast<Element*>(areaNodes.front());
			string areaType = getAttributeByName(areaEl->get_attributes(), "type");
			if( areaType == "box" )
			{
				LOG_DEBUG("Initial state area: " << areaType);
				float minX = atof( getAttributeByName(areaEl->get_attributes(), "minX").c_str() );
				float maxX = atof( getAttributeByName(areaEl->get_attributes(), "maxX").c_str() );
				float minY = atof( getAttributeByName(areaEl->get_attributes(), "minY").c_str() );
				float maxY = atof( getAttributeByName(areaEl->get_attributes(), "maxY").c_str() );
				float minZ = atof( getAttributeByName(areaEl->get_attributes(), "minZ").c_str() );
				float maxZ = atof( getAttributeByName(areaEl->get_attributes(), "maxZ").c_str() );
				LOG_DEBUG("Box size: [" << minX << ", " << maxX << "] " 
									<< "[" << minY << ", " << maxY << "] " 
									<< "[" << minZ << ", " << maxZ << "]");
				stateArea = new BoxArea(minX, maxX, minY, maxY, minZ, maxZ);
			} else {
				LOG_WARN("Unknown initial state area: " << areaType);
			}
		}
		xmlpp::Node::NodeList valuesNodes = stateEl->get_children("values");
		if (valuesNodes.size() > 1)
			THROW_INVALID_INPUT("Only one values element allowed for initial state");
		if (valuesNodes.size()) {
			memset(values, 0, 9*sizeof(float));
			Element *valuesEl = dynamic_cast<Element*>(valuesNodes.front());
			string vx = valuesEl->get_attribute_value("vx");
			if( !vx.empty() )
				values[0] = atof( vx.c_str() );
			string vy = valuesEl->get_attribute_value("vy");
			if( !vy.empty() )
				values[1] = atof( vy.c_str() );
			string vz = valuesEl->get_attribute_value("vz");
			if( !vz.empty() )
				values[2] = atof( vz.c_str() );
			string sxx = valuesEl->get_attribute_value("sxx");
			if( !sxx.empty() )
				values[3] = atof( sxx.c_str() );
			string sxy = valuesEl->get_attribute_value("sxy");
			if( !sxy.empty() )
				values[4] = atof( sxy.c_str() );
			string sxz = valuesEl->get_attribute_value("sxz");
			if( !sxz.empty() )
				values[5] = atof( sxz.c_str() );
			string syy = valuesEl->get_attribute_value("syy");
			if( !syy.empty() )
				values[6] = atof( syy.c_str() );
			string syz = valuesEl->get_attribute_value("syz");
			if( !syz.empty() )
				values[7] = atof( syz.c_str() );
			string szz = valuesEl->get_attribute_value("szz");
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
	log4cxx::BasicConfigurator::configure();
	Engine* engine = new Engine();
	USE_LOGGER;
	INIT_LOGGER("gcm");
	try {
		loadSceneFromFile(engine, "test.xml");
		
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
