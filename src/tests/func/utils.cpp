#include "utils.h"

#include "launcher/launcher.h"
#include "node/CalcNode.h"
#include "Math.h"

Engine& loadTaskScenario(std::string taskFile) {
	Engine& engine = Engine::getInstance();
	engine.setGmshVerbosity(0.0);
	engine.clear();
	engine.getFileLookupService().addPath(".");
	launcher::loadSceneFromFile(engine, taskFile);
	return engine;
}

TetrFirstOrder* findTetr(TetrMesh* m, float x, float y, float z) {
	for (int i = 0; i < m->getTetrsNumber(); i++) {
		TetrFirstOrder& t = m->getTetrByLocalIndex(i);
		if (pointInTetr(x, y, z, m->getNode(t.verts[0]).coords, m->getNode(t.verts[1]).coords, m->getNode(t.verts[2]).coords, m->getNode(t.verts[3]).coords, false))
			return &t;
	}

	return nullptr;
}
