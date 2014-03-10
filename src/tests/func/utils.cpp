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

std::string getDataFileName(int stepNum)
{
	return getTestDataDirName() + "/step" + std::to_string(stepNum) + ".data";
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

void dumpPoint(CalcNode& analytical, CalcNode& numerical, SnapshotLine line, int stepNum)
{
	ofstream datafile;
	datafile.open( getDataFileName(stepNum), ios::app );
	
	float dist = vectorNorm(
		analytical.coords[0] - line.startPoint[0],
		analytical.coords[1] - line.startPoint[1],
		analytical.coords[2] - line.startPoint[2]
	);
	
	// Dump data to file
	datafile << dist << " ";
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

void drawValues(std::initializer_list<std::string> valuesToDraw, int stepNum, std::vector<ValueLimit> *valueLimits)
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
		std::string dataFileName = getDataFileName(stepNum);
		std::string pngFileName = getTestDataDirName() + "/" + values[i] + "." + std::to_string(stepNum) + ".png";
		
		// Set term and target png file
		std::string cmd = std::string("set term pngcairo; ")
							+ "set output " + '"' + pngFileName + '"' + "; ";
		// Set yrange if exists
		if( valueLimits != NULL )
		{
			float min = (*valueLimits)[i].min;
			float max = (*valueLimits)[i].max;
			float order = 1;
			while( fabs(max - min) / order >= 10 )
				order *= 10;
			int imax = (int)( (max + order) / order ) * order;
			int imin = (int)( (min - order) / order ) * order;
			cmd = cmd + "set yrange [" + std::to_string(imin) + ":" + std::to_string(imax) + "]; ";
		}
		// Plot command itself
		cmd = cmd + "plot \"" + dataFileName + '"'
							+ " using 1:" + std::to_string(dataColumnIndex) + " with lines " 
							+ "title " + '"' + values[i] + " analytical" + '"' + ", " 
							+ '"' + dataFileName + '"' 
							+ " using 1:" + std::to_string(dataColumnIndex+1) + " with lines " 
							+ "title " + '"' + values[i] + " numerical" + '"';
		
		// Execute
		fprintf(gnuplot, "%s\n", cmd.c_str());
	}
	
	fprintf(gnuplot, "exit\n");
	fclose(gnuplot);
}

// TODO: use mesh-independent API for arbitrary point interpolation (instead of hardcoded mesh type, body name, mesh name)
// TODO: change norm (current norm depends on number of points too heavily)
void runTaskAsTest(std::string taskFile, void(*setAnalytical)(CalcNode&, float, Engine&), 
					int stepsNum, SnapshotLine line, std::initializer_list<std::string> valuesToDraw, 
					float ALLOWED_VALUE_DEVIATION_PERCENT, int ALLOWED_NUMBER_OF_BAD_NODES )
{
	// Create dir for test data
	// TODO: rethink this ugly solution
	std::string resDirName = getTestDataDirName();
	system(("rm -r " + resDirName).c_str());
	mkpath( resDirName );
	
	float time = 0.0;
	CalcNode node;
	
	std::vector<ValueLimit> valueLimits;
	valueLimits.resize(9);
	for (int k = 0; k < 9; k++)
	{
		valueLimits[k].min = numeric_limits<float>::infinity();
		valueLimits[k].max = - numeric_limits<float>::infinity();
	}
	
	// Load task
	Engine& engine = loadTaskScenario(taskFile);
	
	float dt = engine.calculateRecommendedTimeStep();
	engine.setTimeStep(dt);
	
	// Create snap nodes
	CalcNode snapNodes[line.numberOfPoints];
	// Determine line size
	float dx[3];
	for (int k = 0; k < 3; k++)
		dx[k] = line.endPoint[k] - line.startPoint[k];
	// Calc nodes coords
	for (int i = 0; i < line.numberOfPoints; i++)
		for (int k = 0; k < 3; k++)
			snapNodes[i].coords[k] = line.startPoint[k] + dx[k] * i / (line.numberOfPoints - 1);
	
	// TODO: it should be removed, we need mesh-independent solution
	auto mesh = dynamic_cast<TetrMeshFirstOrder*>(engine.getBodyById("cube")->getMesh("main"));
	TetrFirstOrder* tetrs[line.numberOfPoints];
	for (int i = 0; i < line.numberOfPoints; i++)
	{
		tetrs[i] = findTetr(mesh, snapNodes[i].coords[0], snapNodes[i].coords[1], snapNodes[i].coords[2] );
		ASSERT_TRUE(tetrs[i]);
	}
	
	// Calc velocity and pressure norm based on initial state
	float velocityNorm = - numeric_limits<float>::infinity();
	float pressureNorm = - numeric_limits<float>::infinity();
	for (int i = 0; i < line.numberOfPoints; i++)
	{
		node.coords[0] = snapNodes[i].coords[0];
		node.coords[1] = snapNodes[i].coords[1];
		node.coords[2] = snapNodes[i].coords[2];
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
		for (int i = 0; i < line.numberOfPoints; i++)
		{
			// 'node' contains analytical solution
			node.coords[0] = snapNodes[i].coords[0];
			node.coords[1] = snapNodes[i].coords[1];
			node.coords[2] = snapNodes[i].coords[2];
			setAnalytical(node, time, engine);
			
			// 'snapNode[i]' contains numerical solution
			bool isInnerPoint;
			CalcNode& baseNode = mesh->getNode(tetrs[i]->verts[0]);
			mesh->interpolateNode(
					baseNode, 
					snapNodes[i].x - baseNode.x, 
					snapNodes[i].y - baseNode.y, 
					snapNodes[i].z - baseNode.z, 
					false, 
					snapNodes[i], 
					isInnerPoint
			);
			ASSERT_TRUE( isInnerPoint );
			
			// Dump data to file
			dumpPoint(node, snapNodes[i], line, t);
			
			// Check values
			for(int v = 0; v < GCM_MATRIX_SIZE; v++ )
			{
				float delta = fabs(node.values[v] - snapNodes[i].values[v]);
				float norm = (v < 3 ? velocityNorm : pressureNorm);
				if( delta > norm * ALLOWED_VALUE_DEVIATION_PERCENT )
				{
					badNodes++;
					LOG_INFO("Bad nodes: " << node << "\n" << "VS" << snapNodes[i]);
					LOG_INFO("Compare values[" << v << "], delta " << delta << ", norm " << norm );
					break;
				}
			}
			
			// Update limits
			for(int v = 0; v < GCM_MATRIX_SIZE; v++ )
			{
				if( snapNodes[i].values[v] < valueLimits[v].min )
					valueLimits[v].min = snapNodes[i].values[v];
				if( snapNodes[i].values[v] > valueLimits[v].max )
					valueLimits[v].max = snapNodes[i].values[v];
			}
		}
		
		//Draw values here and now to have graphs if test fails
		drawValues(valuesToDraw, t, NULL);
		
		ASSERT_LE(badNodes, ALLOWED_NUMBER_OF_BAD_NODES);
		
		// Calc next state
		engine.doNextStep();
	}
	
	// Draw values again, override previous pics
	// We pass actual valueLimits to get the same yrange for all graphs
	for (int t = 0; t <= stepsNum; t++)
		drawValues(valuesToDraw, t, &valueLimits);
}