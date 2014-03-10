#include "utils.h"

#include "launcher/launcher.h"
#include "node/CalcNode.h"
#include "Math.h"

// TODO: may be just use boost for (a) mkdir, (b) rm -r, (c) case insensitive string match

#include <iostream>
#include <fstream>
#include <stdlib.h>
// TODO: it's Linux only
#include <sys/stat.h>

// TODO: it's Linux only
int mkpath(std::string s)
{
	mode_t mode = 0755;
	size_t pre=0,pos;
	std::string dir;
	int mdret;

	if(s[s.size()-1]!='/')
	{
		// force trailing / so we can handle everything in loop
		s+='/';
	}
	
	while((pos=s.find_first_of('/',pre))!=std::string::npos)
	{
		dir=s.substr(0,pos++);
		pre=pos;
		if(dir.size()==0) continue; // if leading / first time is 0 length
		if((mdret=mkdir(dir.c_str(),mode)) && errno!=EEXIST)
		{
			return mdret;
		}
	}
	return 0;
}

std::string getTestDataDirName()
{
	const ::testing::TestInfo* const testInfo = ::testing::UnitTest::GetInstance()->current_test_info();
	return "!testsRun/" + std::string(testInfo->test_case_name()) + "/" + std::string(testInfo->name());
}

Engine& loadTaskScenario(std::string taskFile) {
	Engine& engine = Engine::getInstance();
	engine.setGmshVerbosity(0.0);
	engine.clear();
	engine.getFileLookupService().addPath(".");
	launcher::loadSceneFromFile(engine, taskFile);
	return engine;
}

// TODO: remove it
TetrFirstOrder* findTetr(TetrMesh* m, float x, float y, float z) {
	for (int i = 0; i < m->getTetrsNumber(); i++) {
		TetrFirstOrder& t = m->getTetrByLocalIndex(i);
		if (pointInTetr(x, y, z, m->getNode(t.verts[0]).coords, m->getNode(t.verts[1]).coords, m->getNode(t.verts[2]).coords, m->getNode(t.verts[3]).coords, false))
			return &t;
	}

	return nullptr;
}

void dumpPoint(CalcNode& analytical, CalcNode& numerical, int stepNum)
{
	ofstream datafile;
	std::string resDirName = getTestDataDirName();
	datafile.open(resDirName + "/step" + std::to_string(stepNum) + ".data", ios::app);
	
	// Dump data to file
	datafile << analytical.z << " ";
	for(int v = 0; v < GCM_MATRIX_SIZE; v++ )
		datafile << analytical.values[v] << " " << numerical.values[v] << " ";
	datafile << "\n";
	
	datafile.close();
}

bool shouldDraw(std::string value, std::initializer_list<std::string> valuesToDraw)
{
	auto d = valuesToDraw.begin();
	while (d != valuesToDraw.end())
		if( value == *(d++))
			return true;
	return false;
}

void drawValues(std::initializer_list<std::string> valuesToDraw, int stepNum)
{
	FILE* gnuplot = popen("gnuplot", "w");
	if ( gnuplot == 0 )
	{
		LOG_WARN( "No gnuplot available!" );
		return;
    }
	
	vector<std::string> values = {"vx", "vy", "vz", "sxx", "sxy", "sxz", "syy", "syz", "szz"};
	for(unsigned int i = 0; i < values.size(); i++)
	{
		if( ! shouldDraw(values[i], valuesToDraw) )
			continue;
		
		int dataColumnIndex = (i+1)*2;
		std::string dataFileName = getTestDataDirName() + "/step" + std::to_string(stepNum) + ".data";
		std::string pngFileName = getTestDataDirName() + "/" + values[i] + "." + std::to_string(stepNum) + ".png";
		
		std::string cmd = std::string("set term pngcairo; ")
							+ "set output " + '"' + pngFileName + '"' + "; " 
							+ "plot \"" + dataFileName + '"'
							+ " using 1:" + std::to_string(dataColumnIndex) + " with lines " 
							+ "title " + '"' + values[i] + " analytical" + '"' + ", " 
							+ '"' + dataFileName + '"' 
							+ " using 1:" + std::to_string(dataColumnIndex+1) + " with lines " 
							+ "title " + '"' + values[i] + " numerical" + '"' + ";";
		fprintf(gnuplot, "%s\n", cmd.c_str());
	}
	
	fprintf(gnuplot, "exit\n");
	fclose(gnuplot);
}

// TODO: pass start point, end point, number of points (instead of just number of points and hardcoded start and end)
// TODO: use mesh-independent API for arbitrary point interpolation (instead of hardcoded mesh type, body name, mesh name)
// TODO: change norm (current norm depends on number of points too heavily)
void runTaskAsTest(std::string taskFile, void(*setAnalytical)(CalcNode&, float, Engine&), 
					int stepsNum, int pointsNum, 
					float ALLOWED_VALUE_DEVIATION_PERCENT, int ALLOWED_NUMBER_OF_BAD_NODES, 
					std::initializer_list<std::string> valuesToDraw)
{
	// Create dir for test data
	// TODO: rethink this ugly solution
	std::string resDirName = getTestDataDirName();
	system(("rm -r " + resDirName).c_str());
	mkpath( resDirName );
	
	float time = 0.0;
	CalcNode node, cnode;
	
	// Load task
	Engine& engine = loadTaskScenario(taskFile);
	
	float dt = engine.calculateRecommendedTimeStep();
	engine.setTimeStep(dt);
	
	// TODO: it should be removed, we need mesh-independent solution
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
	
	// Calc velocity and pressure norm based on initial state
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
	
	// Do time steps
	for (int t = 0; t <= stepsNum; t++, time += dt)
	{
		// Check numerical solutions
		int badNodes = 0;
		for (int i = 0; i < pointsNum; i++)
		{
			cnode.z = node.z = z[i];
			
			// 'node' contains analytical solution
			setAnalytical(node, time, engine);
			
			// 'cnode' contains numerical solution
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
			
			// Dump data to file
			dumpPoint(node, cnode, t);
			
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
		
		drawValues(valuesToDraw, t);
		ASSERT_LE(badNodes, ALLOWED_NUMBER_OF_BAD_NODES);
		
		// Calc next state
		engine.doNextStep();
	}
}