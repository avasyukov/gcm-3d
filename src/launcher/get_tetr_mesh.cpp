#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <unistd.h>

#include <unordered_map>

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

#include "libgcm/mesh/euler/markers/MarkeredMesh.hpp"
#include "libgcm/mesh/tetr/TetrMeshFirstOrder.hpp"

#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLStructuredGridWriter.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkSmartPointer.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkTetra.h>

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
using std::unordered_map;

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

    try
    {
        po::options_description desc("Program Usage", 1024, 512);

        auto taskFileOption = TYPED_VALUE(taskFile);
        taskFileOption->value_name("task")->required();

        auto dataDirOption = TYPED_VALUE(dataDir);
        dataDirOption->value_name("data")->default_value(".");

        desc.add_options()
              ("task,t"                     , taskFileOption         , "xml file with task description")
              ("data-dir,d"                 , dataDirOption          , "directory with models specified in task")
         ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help"))
        {
            cout << desc << "\n";
            return 0;
        }

        po::notify(vm);

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

        launcher::Launcher launcher;
        //launcher.loadMaterialLibrary("materials");
        launcher.loadSceneFromFile(taskFile, initialStateGroup);

        auto mesh = dynamic_cast<MarkeredMesh*>(engine.getBody(0)->getMeshes());

        auto dims = mesh->getNodeDimensions();

        unordered_map<uint, uint> ind;

        auto grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
        auto points = vtkSmartPointer<vtkPoints>::New();

        auto matId = vtkSmartPointer<vtkIntArray>::New();
        matId->SetName("materialID");

        auto nodeNumber = vtkSmartPointer<vtkIntArray>::New ();
        nodeNumber->SetName ("nodeNumber");

        int nn = 0;
        for (uint i = 0; i < dims.x; i++)
            for (uint j = 0; j < dims.y; j++)
                for (uint k = 0; k < dims.z; k++){
                    auto& node = mesh->getNodeByEulerMeshIndex(vector3u(i,j,k));
                    if (node.isUsed()){
                        int li = mesh->getNodeLocalIndexByEulerMeshIndex(vector3u(i,j,k));
                        ind[li] = nn;
                        nn++;

                        matId->InsertNextValue(node.getMaterialId());
                        nodeNumber->InsertNextValue(nn);
                        points->InsertNextPoint( node.coords[0], node.coords[1], node.coords[2] );
                    }
                }

        dims = mesh->getDimensions();

        auto tetra=vtkSmartPointer<vtkTetra>::New();
        for (uint i = 0; i < dims.x; i++)
            for (uint j = 0; j < dims.y; j++)
                for (uint k = 0; k < dims.z; k++)
                    if (mesh->getCellStatus(vector3u(i,j,k))){
                        uint i000 = ind[mesh->getNodeLocalIndexByEulerMeshIndex(vector3u(i+0,j+0,k+0))];
                        uint i001 = ind[mesh->getNodeLocalIndexByEulerMeshIndex(vector3u(i+0,j+0,k+1))];
                        uint i010 = ind[mesh->getNodeLocalIndexByEulerMeshIndex(vector3u(i+0,j+1,k+0))];
                        uint i011 = ind[mesh->getNodeLocalIndexByEulerMeshIndex(vector3u(i+0,j+1,k+1))];
                        uint i100 = ind[mesh->getNodeLocalIndexByEulerMeshIndex(vector3u(i+1,j+0,k+0))];
                        uint i101 = ind[mesh->getNodeLocalIndexByEulerMeshIndex(vector3u(i+1,j+0,k+1))];
                        uint i110 = ind[mesh->getNodeLocalIndexByEulerMeshIndex(vector3u(i+1,j+1,k+0))];
                        uint i111 = ind[mesh->getNodeLocalIndexByEulerMeshIndex(vector3u(i+1,j+1,k+1))];

                        uint j1 = i001;
                        uint j2 = i011;
                        uint j3 = i111;
                        uint j4 = i101;
                        uint j5 = i000;
                        uint j6 = i010;
                        uint j7 = i110;
                        uint j8 = i100;


                        tetra->GetPointIds()->SetId(0, j1);
                        tetra->GetPointIds()->SetId(1, j2);
                        tetra->GetPointIds()->SetId(2, j4);
                        tetra->GetPointIds()->SetId(3, j6);
                        grid->InsertNextCell(tetra->GetCellType(),tetra->GetPointIds());

                        tetra->GetPointIds()->SetId(0, j1);
                        tetra->GetPointIds()->SetId(1, j4);
                        tetra->GetPointIds()->SetId(2, j6);
                        tetra->GetPointIds()->SetId(3, j8);
                        grid->InsertNextCell(tetra->GetCellType(),tetra->GetPointIds());

                        tetra->GetPointIds()->SetId(0, j1);
                        tetra->GetPointIds()->SetId(1, j5);
                        tetra->GetPointIds()->SetId(2, j6);
                        tetra->GetPointIds()->SetId(3, j8);
                        grid->InsertNextCell(tetra->GetCellType(),tetra->GetPointIds());

                        tetra->GetPointIds()->SetId(0, j2);
                        tetra->GetPointIds()->SetId(1, j3);
                        tetra->GetPointIds()->SetId(2, j4);
                        tetra->GetPointIds()->SetId(3, j6);
                        grid->InsertNextCell(tetra->GetCellType(),tetra->GetPointIds());

                        tetra->GetPointIds()->SetId(0, j3);
                        tetra->GetPointIds()->SetId(1, j4);
                        tetra->GetPointIds()->SetId(2, j6);
                        tetra->GetPointIds()->SetId(3, j8);
                        grid->InsertNextCell(tetra->GetCellType(),tetra->GetPointIds());

                        tetra->GetPointIds()->SetId(0, j3);
                        tetra->GetPointIds()->SetId(1, j6);
                        tetra->GetPointIds()->SetId(2, j7);
                        tetra->GetPointIds()->SetId(3, j8);
                        grid->InsertNextCell(tetra->GetCellType(),tetra->GetPointIds());
                    }

        vtkFieldData* fd = grid->GetPointData();

        grid->SetPoints(points);

        fd->AddArray(matId);
        fd->AddArray(nodeNumber);

        // Write file
        auto writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
        writer->SetFileName("tetr_mesh.vtu");
        writer->SetInputData(grid);
        writer->Write();

        engine.cleanUp();
        GmshFinalize();

    } catch (Exception &e) {
        LOG_FATAL("Exception was thrown: " << e.getMessage() << "\n @" << e.getFile() << ":" << e.getLine() << "\nCall stack: \n"<< e.getCallStack());
    }
}
