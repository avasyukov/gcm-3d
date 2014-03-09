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

// TODO: pass start point, end point, number of points (instead of just number of points and hardcoded start and end)
// TODO: use mesh-independent API for arbitrary point interpolation (instead of hardcoded mesh type, body name, mesh name)
// TODO: change norm (current norm depends on number of points too heavily)
void runTaskAsTest(std::string taskFile, void(*setAnalytical)(CalcNode&, float, Engine&), 
					int stepsNum, int pointsNum, 
					float ALLOWED_VALUE_DEVIATION_PERCENT, float ALLOWED_NUMBER_OF_BAD_NODES)
{
	float time = 0.0;
	CalcNode node, cnode;

	Engine& engine = loadTaskScenario(taskFile);
	
	float dt = engine.calculateRecommendedTimeStep();
	engine.setTimeStep(dt);
	
	auto mesh = dynamic_cast<TetrMeshFirstOrder*>(engine.getBodyById("cube")->getMesh("main"));

	TetrFirstOrder* tetrs[pointsNum];
	float z[pointsNum];

	for (int i = 0; i < pointsNum; i++)
	{
		z[i] = -5.0 + 10.0*i/(pointsNum-1);
		tetrs[i] = findTetr(mesh, 0.0, 0.0, z[i]);
		ASSERT_TRUE(tetrs[i]);
	}
	
	cnode.x = node.x = 0.0;
	cnode.y = node.y = 0.0;
	
	float velocityNorm = - numeric_limits<float>::infinity();
	float pressureNorm = - numeric_limits<float>::infinity();
	for (int i = 0; i < pointsNum; i++)
	{
		node.z = z[i];
		setAnalytical(node, 0.0, engine);
		// Check velocity
		for(int v = 0; v < 3; v++ )
		{
			if( fabs(node.values[v]) > velocityNorm )
				velocityNorm = fabs(node.values[v]);
		}
		// Check pressure
		for(int v = 3; v < GCM_MATRIX_SIZE; v++ )
		{
			if( fabs(node.values[v]) > pressureNorm )
				pressureNorm = fabs(node.values[v]);
		}
	}
	
	for (int t = 0; t < stepsNum; t++, time += dt)
	{
		int badNodes = 0;
		engine.doNextStep();
		for (int i = 0; i < pointsNum; i++)
		{
			cnode.z = node.z = z[i];

			setAnalytical(node, time, engine);

			bool isInnerPoint;
			CalcNode& baseNode = mesh->getNode(tetrs[i]->verts[0]);
			mesh->interpolateNode(
					baseNode, 
					cnode.x - baseNode.x, 
					cnode.y - baseNode.y, 
					cnode.z - baseNode.z, 
					false, 
					cnode, 
					isInnerPoint
			);
			ASSERT_TRUE( isInnerPoint );
			
			// Check values
			for(int v = 0; v < GCM_MATRIX_SIZE; v++ )
			{
				float delta = fabs(node.values[v] - cnode.values[v]);
				float norm = (v < 3 ? velocityNorm : pressureNorm);
				if( delta > norm * ALLOWED_VALUE_DEVIATION_PERCENT )
				{
					badNodes++;
					LOG_INFO("Bad nodes: " << node << "\n" << "VS" << cnode);
					LOG_INFO("Compare values[" << v << "], delta " << delta << ", norm " << norm );
					break;
				}
			}
		}
		
		ASSERT_LE(badNodes, ALLOWED_NUMBER_OF_BAD_NODES);
	}
}