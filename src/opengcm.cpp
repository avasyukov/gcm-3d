#include <iostream>
#include <string>
#include <log4cxx/basicconfigurator.h>

#include "Engine.h"
#include "mesh/Mesh.h"
#include "Logging.h"

using namespace std;
using namespace gcm;

int main(int argc, char **argv, char **envp)
{
	log4cxx::BasicConfigurator::configure();
	Engine* engine = new Engine();
	USE_LOGGER;
	INIT_LOGGER("gcm");
	try {
		engine->loadSceneFromFile("test.xml");
		
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
