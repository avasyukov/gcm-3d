#include <iostream>
#include <string>
#include <log4cxx/basicconfigurator.h>

#include "Engine.h"
#include "mesh/Mesh.h"
#include "Logging.h"
#include "snapshot/VTKSnapshotWriter.h"

using namespace std;
using namespace gcm;

int main(int argc, char **argv, char **envp)
{
	log4cxx::BasicConfigurator::configure();
	Engine engine;
	VTKSnapshotWriter* sw = new VTKSnapshotWriter();
	USE_LOGGER;
	INIT_LOGGER("gcm");
	try {
		engine.loadSceneFromFile("test.xml");
		
		TetrMeshFirstOrder* mesh = (TetrMeshSecondOrder*)engine.getBody(0)->getMeshes();
		mesh->setRheology(70000, 10000, 1);
		
		engine.setTimeStep( mesh->getRecommendedTimeStep() );
		//engine.setTimeStep( 2 * mesh->getMaxPossibleTimeStep() );
		
		int numberOfSteps = 20;
		for( int i = 0; i < numberOfSteps; i++ )
		{
			LOG_INFO( "Dumping mesh " << mesh->getId() );
			sw->dump(mesh, i);
			
			engine.doNextStep();
		}
		
		LOG_DEBUG( "Dumping mesh " << mesh->getId() );
		sw->dump(mesh, numberOfSteps);
	} catch (Exception &e) {
		LOG_FATAL("Exception was thrown: " << e.getMessage() << "\n @" << e.getFile() << ":" << e.getLine() << "\nCall stack: \n"<< e.getCallStack());
	}
}	
