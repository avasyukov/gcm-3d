#include "tests/func/util.hpp"

#include "launcher/launcher.hpp"
#include "launcher/util/FileFolderLookupService.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/Math.hpp"
#include "libgcm/Logging.hpp"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdlib.h>

using namespace gcm;
using namespace std;

bfs::path getTestDataDirName()
{
    const ::testing::TestInfo* const testInfo = ::testing::UnitTest::GetInstance()->current_test_info();
    bfs::path path(".");
    return path / "!testsRun" / std::string(testInfo->test_case_name()) / std::string(testInfo->name());
}

bfs::path getDataFileName(int stepNum)
{
    return getTestDataDirName() / ("step" + std::to_string(stepNum) + ".data");
}

void dumpPoint(CalcNode& analytical, CalcNode& numerical, SnapshotLine line, int stepNum)
{
    ofstream datafile;
    datafile.open(getDataFileName(stepNum).string(), ios::app);

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

bool shouldDraw(std::string value, std::vector<std::string> valuesToDraw)
{
    return std::find(valuesToDraw.begin(), valuesToDraw.end(), value) != valuesToDraw.end();
}

void drawValues(std::vector<std::string> valuesToDraw, int stepNum, std::vector<ValueLimit> *valueLimits)
{
    FILE* gnuplot = popen("gnuplot", "w");
    if ( gnuplot == 0 )
    {
        LOG_WARN( "No gnuplot available!" );
        return;
    }

    vector<std::string> values {"vx", "vy", "vz", "sxx", "sxy", "sxz", "syy", "syz", "szz"};
    for(unsigned int i = 0; i < values.size(); i++)
    {
        if( ! shouldDraw(values[i], valuesToDraw) )
            continue;

        int dataColumnIndex = (i+1)*2;
        bfs::path dataFileName = getDataFileName(stepNum);
        bfs::path pngFileName = getTestDataDirName() / (values[i] + "." + std::to_string(stepNum) + ".png");

        // Set term and target png file
        std::string cmd = std::string("set term pngcairo; ")
                            + "set output " + '"' + pngFileName.string() + '"' + "; ";
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
        cmd = cmd + "plot \"" + dataFileName.string() + '"'
                            + " using 1:" + std::to_string(dataColumnIndex) + " with lines "
                            + "title " + '"' + values[i] + " analytical" + '"' + ", "
                            + '"' + dataFileName.string() + '"'
                            + " using 1:" + std::to_string(dataColumnIndex+1) + " with lines "
                            + "title " + '"' + values[i] + " numerical" + '"';

        // Execute
        fprintf(gnuplot, "%s\n", cmd.c_str());
    }

    fprintf(gnuplot, "exit\n");
    fclose(gnuplot);
}

// TODO: change norm (current norm depends on number of points too heavily)
void runTaskAsTest(std::string taskFile, void(*setAnalytical)(CalcNode&, float, Engine&),
                    int stepsNum, SnapshotLine line, std::vector<std::string> valuesToDraw,
                    float ALLOWED_VALUE_DEVIATION_PERCENT, int ALLOWED_NUMBER_OF_BAD_NODES )
{
    USE_AND_INIT_LOGGER("gcm.tests.func.runTaskAsTest");
    try
    {
        // Create dir for test data
        // TODO: rethink this ugly solution
        bfs::path resDirName = getTestDataDirName();
        bfs::remove_all(resDirName);
        bfs::create_directories(resDirName);

        float time = 0.0;
        CalcNode node;

        std::vector<ValueLimit> valueLimits;
        valueLimits.resize(9);
        for (int k = 0; k < 9; k++)
        {
            valueLimits[k].min = numeric_limits<float>::infinity();
            valueLimits[k].max = - numeric_limits<float>::infinity();
        }

        // load material library
        launcher::Launcher launcher;
        // Load task
        Engine& engine = Engine::getInstance();
        
        engine.setGmshVerbosity(0.0);
        engine.clear();
        launcher::FileFolderLookupService::getInstance().addPath(".");
        launcher.loadMaterialLibrary("materials");
        launcher.loadSceneFromFile(taskFile);

        float dt = engine.calculateRecommendedTimeStep();
        engine.setTimeStep(dt);

        // Create snap nodes
        CalcNode* snapNodes = new CalcNode[line.numberOfPoints];
        // Determine line size
        float dx[3];
        for (int k = 0; k < 3; k++)
            dx[k] = line.endPoint[k] - line.startPoint[k];
        // Calc nodes coords
        for (int i = 0; i < line.numberOfPoints; i++)
            for (int k = 0; k < 3; k++)
                snapNodes[i].coords[k] = line.startPoint[k] + dx[k] * i / (line.numberOfPoints - 1);

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

        int badTimeSteps = 0;

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
                bool interpolated = engine.interpolateNode(snapNodes[i]);
                ASSERT_TRUE(interpolated);

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

            LOG_INFO("Step " << t << ". Analyze complete. Bad nodes number: " << badNodes);
            if( badNodes > ALLOWED_NUMBER_OF_BAD_NODES)
                badTimeSteps++;

            // Calc next state
            engine.doNextStep();
        }

        // Draw values again, override previous pics
        // We pass actual valueLimits to get the same yrange for all graphs
        for (int t = 0; t <= stepsNum; t++)
            drawValues(valuesToDraw, t, &valueLimits);

        ASSERT_EQ(badTimeSteps, 0);

        delete[] snapNodes;
    }
    catch (Exception &e)
    {
        LOG_FATAL("Exception was thrown: " << e.getMessage() << "\n @" << e.getFile() << ":" << e.getLine() << "\nCall stack: \n"<< e.getCallStack());
        throw;
    }
}
