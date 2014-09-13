#include <iostream>
#include <string>
#include <vector>
#include <exception>

#include <getopt.h>

#include "libgcm/config.hpp"

#if CONFIG_ENABLE_LOGGING
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/mdc.h>
#endif

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/mpi.hpp>
#include <boost/serialization/vector.hpp>
#include "launcher/util/serialize_tuple.hpp"

#include <gmsh/Gmsh.h>

#include "launcher/launcher.hpp"
#include "launcher/util/FileFolderLookupService.hpp"
#include "libgcm/Engine.hpp"

using namespace std;
using namespace gcm;
using namespace launcher;
using namespace boost::filesystem;
using boost::property_tree::ptree;
using boost::property_tree::write_json;

namespace mpi = boost::mpi;



// This function print usage message
void print_help(char* binaryName)
{
    cout << "\nUsage: " << binaryName << " --task file --data-dir dir\n"
        << "\t--task - xml file with task description\n"
        << "\t--data-dir - directory with models specified in task\n";
};

int main(int argc, char **argv, char **envp)
{
    FileFolderLookupService fls;

    // Parse command line options
    int c;
    static struct option long_options[] =
    {
        {"task"      , required_argument, 0, 't'},
        {"data-dir"  , required_argument, 0, 'd'},
        {"help"      , no_argument      , 0, 'h'},
        {0           , 0                , 0, 0  }
    };
    int option_index = 0;

    string taskFile;
    string dataDir;

    while ((c = getopt_long (argc, argv, "t:d:h:", long_options, &option_index)) != -1)
    {
        switch (c)
        {
            case 't':
                taskFile = optarg;
                break;
            case 'd':
                dataDir = optarg;
                if(dataDir[dataDir.length()-1] == '/')
                    dataDir[dataDir.length()-1] = '\0';
                break;
            case 'h':
                print_help(argv[0]);
                return 0;
            case '?':
                print_help(argv[0]);
            default:
                return -1;
        }
    }

    USE_AND_INIT_LOGGER("gcm");

    try {
        fls.addPath(CONFIG_SHARE_GCM);
        if( dataDir.empty() )
            dataDir = CONFIG_SHARE_GCM;
        fls.addPath(dataDir);
        fls.addPath("./src/launcher/");


        mpi::environment env;
        mpi::communicator world;

        GmshInitialize();
        #if CONFIG_ENABLE_LOGGING
        char pe[5];
        sprintf(pe, "%d", world.rank());
        log4cxx::MDC::put("PE", pe);
        log4cxx::PropertyConfigurator::configure(fls.lookupFile("log4cxx.properties"));
        #endif
    
        if( taskFile.empty() )
            THROW_INVALID_ARG("No task file provided");
        LOG_INFO("Starting with taskFile '" << taskFile << "' and dataDir '" << dataDir << "'");

        Engine& engine = Engine::getInstance();
        FileFolderLookupService::getInstance().addPath(dataDir);
        launcher::Launcher launcher;
        //launcher.loadMaterialLibrary("materials");
        launcher.loadSceneFromFile(taskFile);
        engine.calculate();

        if (world.rank() == 0)
        {
            vector<vector<tuple<unsigned int, string, string>>> snapshots;
            mpi::gather(world, engine.getSnapshotsList(), snapshots, 0);

            ofstream snapListFile(path(taskFile).filename().string() + ".snapshots");
            ptree snaps, root;

            const auto& timestamps = engine.getSnapshotTimestamps();

            for (int i = 0; i < timestamps.size(); i++)
            {
                ptree stepSnaps, list;
                stepSnaps.put<int>("index", i);
                stepSnaps.put<float>("time", timestamps[i]);
                for (int worker = 0; worker <  snapshots.size(); worker++)
                {
                    for (auto snapInfo: snapshots[worker])
                    {
                        auto step = get<0>(snapInfo);
                        auto meshId = get<1>(snapInfo);
                        auto snapName = get<2>(snapInfo);

                        if (step == i)
                        {
                            ptree snap;
                            snap.put<string>("mesh", meshId);
                            snap.put<string>("file", snapName);
                            snap.put<int>("worker", worker);
                            list.push_back(make_pair("", snap));

                        }
                        else
                            if (step > i)
                                break;
                    }
                }
                stepSnaps.put_child("snaps", list);
                snaps.push_back(make_pair("", stepSnaps));
            }

            root.put_child("snapshots", snaps);
            write_json(snapListFile, root);
        }
        else
            mpi::gather(world, engine.getSnapshotsList(), 0);



        engine.cleanUp();
        GmshFinalize();
    } catch (Exception &e) {
        LOG_FATAL("Exception was thrown: " << e.getMessage() << "\n @" << e.getFile() << ":" << e.getLine() << "\nCall stack: \n"<< e.getCallStack());
    }
}
