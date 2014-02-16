#include "utils.h"

#include "../launcher/launcher.h"

Engine& loadTaskScenario(std::string taskFile) {
	Engine& engine = Engine::getInstance();
	engine.setGmshVerbosity(0.0);
	engine.clear();
	engine.getFileLookupService().addPath(".");
	launcher::loadSceneFromFile(engine, taskFile);
	return engine;
}
