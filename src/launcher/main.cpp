#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <unistd.h>

#include "libgcm/config.hpp"

#if CONFIG_ENABLE_LOGGING
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/mdc.h>
#endif

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/mpi.hpp>
#include <boost/serialization/vector.hpp>
#include "launcher/util/serialize_tuple.hpp"

#include <gmsh/Gmsh.h>

#include "launcher/launcher.hpp"
#include "launcher/util/FileFolderLookupService.hpp"
#include "libgcm/Engine.hpp"

#include <stdlib.h>

using namespace gcm;
using namespace launcher;
using boost::property_tree::ptree;
using boost::property_tree::write_json;

using std::string;
using std::vector;
using std::tuple;
using std::ofstream;
using std::cout;
using std::endl;
using std::exception;
using std::cerr;
using std::get;

namespace mpi = boost::mpi;
namespace po = boost::program_options;
namespace bfs = boost::filesystem;

#define TYPED_VALUE(VAR) new po::typed_value<decltype(VAR)>(&VAR)

int main(int argc, char **argv, char **envp)
{
    string taskFile;
    string initialStateGroup;
    string dataDir;
    string outputDir;
    string renderOutputDir;

    bool render = false;
    bool render_only = false;
    bool save_snapshots = true;

    try
    {
        po::options_description desc("Program Usage", 1024, 512);

        auto taskFileOption = TYPED_VALUE(taskFile);
        taskFileOption->value_name("task")->required();

        auto initialStateGroupOption = TYPED_VALUE(initialStateGroup);
        initialStateGroupOption->value_name("task")->default_value("");

        auto dataDirOption = TYPED_VALUE(dataDir);
        dataDirOption->value_name("data")->default_value(".");

        auto outputDirOption = TYPED_VALUE(outputDir);
        outputDirOption->value_name("out")->default_value(".");
        
        auto renderOutputDirOption = TYPED_VALUE(renderOutputDir);
        renderOutputDirOption->value_name("render-out")->default_value(".")->composing();

        desc.add_options()
              ("help,h"                     ,                          "show this help message and exit")
              ("task,t"                     , taskFileOption         , "xml file with task description")
              ("initial-state-group,i"      , initialStateGroupOption, "initial state group identifier")
              ("data-dir,d"                 , dataDirOption          , "directory with models specified in task")
              ("output-dir,o"               , outputDirOption        , "directory to write snapshots to")
              ("render-output-dir,O"        , renderOutputDirOption  , "directory to write render results to")
              ("render,r"                   ,                          "render results using specified in task description")
              ("render-only,R"              ,                          "render results using specified in task description (skips calculations)")
              ("without-snapshots,w"        ,                          "do not save snapshot files")
         ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help"))
        {
            cout << desc << "\n";
            return 0;
        }

        po::notify(vm);

        if (vm.count("render"))
            render = true;
        if (vm.count("render-only"))
            render = render_only = true;

        if (vm.count("without-snapshots"))
            save_snapshots = false;
    }
    catch(exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return -1;
    }


    USE_AND_INIT_LOGGER("gcm");

    FileFolderLookupService fls;

    try {
        fls.addPath(CONFIG_SHARE_GCM);
        if( dataDir.empty() )
            dataDir = CONFIG_SHARE_GCM;
        fls.addPath(dataDir);
        fls.addPath("./src/launcher/");


        mpi::environment env(argc, argv, false);
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

        auto outputPathPattern = bfs::path(outputDir);

        if (!bfs::is_directory(outputPathPattern))
            bfs::create_directories(outputPathPattern);

        engine.setOption(Engine::Options::SNAPSHOT_OUTPUT_DIRECTORY, outputPathPattern.string());
        outputPathPattern /= bfs::path("snap_mesh_%{MESH}%{SUFFIX}_cpu_%{RANK}_step_%{STEP}.%{EXT}");
        engine.setOption(Engine::Options::SNAPSHOT_OUTPUT_PATH_PATTERN, outputPathPattern.string());
        
        auto snapListFilePath = bfs::path(outputDir);
        snapListFilePath /= bfs::path(taskFile).filename().string() + ".snapshots";

        if (render_only)
            LOG_INFO("Skipping calculation, render-only mode");
        else
        {
            launcher::Launcher launcher;
            //launcher.loadMaterialLibrary("materials");
            launcher.loadSceneFromFile(taskFile, initialStateGroup);
            engine.calculate(save_snapshots);


            if (save_snapshots) {
                if (world.rank() == 0) {
                    vector<vector<tuple<unsigned int, string, string>>> snapshots;
                    mpi::gather(world, engine.getSnapshotsList(), snapshots, 0);

                    ofstream snapListFile(snapListFilePath.string());
                    ptree snaps, root;

                    const auto &timestamps = engine.getSnapshotTimestamps();

                    for (uint i = 0; i < timestamps.size(); i++) {
                        ptree stepSnaps, list;
                        stepSnaps.put<int>("index", i);
                        stepSnaps.put<float>("time", timestamps[i]);
                        for (uint worker = 0; worker < snapshots.size(); worker++) {
                            for (auto snapInfo: snapshots[worker]) {
                                auto step = get<0>(snapInfo);
                                auto meshId = get<1>(snapInfo);
                                auto snapName = get<2>(snapInfo);

                                if (step == i) {
                                    ptree snap;
                                    snap.put<string>("mesh", meshId);
                                    snap.put<string>("file", snapName);
                                    snap.put<int>("worker", worker);
                                    list.push_back(make_pair("", snap));

                                }
                                else if (step > i)
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
            }
            engine.cleanUp();
            GmshFinalize();
        }

        // FIXME this is not portable
        if (render)
        {
            LOG_DEBUG("Running pv_render");
            bfs::path gcm3d = bfs::path(realpath("/proc/self/exe", NULL));
            bfs::path pv_render = gcm3d.parent_path() / "gcm3d_pv_render.py";

            if (!bfs::is_directory(renderOutputDir))
                bfs::create_directories(renderOutputDir);

            execl(
                "/usr/bin/env", "/usr/bin/env",
                "pvbatch",
                pv_render.string().c_str(),
                "--task", taskFile.c_str(),
                "--snap-list", snapListFilePath.string().c_str(),
                "--output-dir", renderOutputDir.c_str(),
                "render-all",
                (char*)NULL
            );
        }
    } catch (Exception &e) {
        LOG_FATAL("Exception was thrown: " << e.getMessage() << "\n @" << e.getFile() << ":" << e.getLine() << "\nCall stack: \n"<< e.getCallStack());
    }
}
