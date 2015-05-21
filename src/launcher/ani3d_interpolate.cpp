#include "libgcm/config.hpp"

#include <string>
#include <vector>
#include <utility>
#include <cmath>

#if CONFIG_ENABLE_LOGGING
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/mdc.h>
#endif

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/mpi.hpp>
#include <boost/lexical_cast.hpp>

#include <vtkXMLStructuredGridReader.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredGridGeometryFilter.h>


#include "libgcm/Logging.hpp"
#include "launcher/util/FileFolderLookupService.hpp"
#include "libgcm/mesh/tetr/TetrMeshFirstOrder.hpp"
#include "libgcm/util/formats/Ani3DTetrFileReader.hpp"
#include "libgcm/Math.hpp"

using std::string;
using std::vector;
using std::pair;
using boost::split;
using boost::is_any_of;
using boost::lexical_cast;
namespace mpi = boost::mpi;

using namespace launcher;
using namespace gcm;

int main(int argc, char** argv)
{
    USE_AND_INIT_LOGGER("gcm.ani3dinterpolate");

    string cubicMesh(argv[1]);
    int snap = lexical_cast<int>(argv[2]);
        
    mpi::environment env(argc, argv, false);
    mpi::communicator world;
    
    FileFolderLookupService fls;

    fls.addPath("./src/launcher/");

    #if CONFIG_ENABLE_LOGGING
    char pe[5];
    sprintf(pe, "%d", world.rank());
    log4cxx::MDC::put("PE", pe);
    log4cxx::PropertyConfigurator::configure(fls.lookupFile("log4cxx.properties"));
    #endif

    LOG_INFO("Structured mesh file: " << cubicMesh);
    LOG_INFO("Snapshot number: " << snap);
    
    Engine& engine = Engine::getInstance();
    Ani3DTetrFileReader reader;

    vtkSmartPointer<vtkXMLStructuredGridReader> vreader = vtkSmartPointer<vtkXMLStructuredGridReader>::New();
    vreader->SetFileName(cubicMesh.c_str());
    vreader->Update();

    auto grid = vreader->GetOutput();
    int* dims = grid->GetDimensions();

    double* bounds = grid->GetBounds();
            
    auto getIndex = [=](int i, int j, int k) { return k*dims[0]*dims[1]+j*dims[0]+i; };

    double dx = (bounds[1]-bounds[0])/(dims[0]-1);
    double dy = (bounds[3]-bounds[2])/(dims[1]-1);
    double dz = (bounds[5]-bounds[4])/(dims[2]-1);

    LOG_INFO("Mesh bounds: [" << bounds[0] << ", " << bounds[1] << "]x[" << bounds[2] << ", " << bounds[3] << "]x[" << bounds[4] << ", " << bounds[5] << "]");
    LOG_INFO("Cell size [" << dx << ", " << dy << ", " << dz << "]");
    LOG_INFO("Dimensions: " << dims[0] << "x" << dims[1] << "x" << dims[2]);
            
    auto pd = grid->GetPointData();
            
    auto sxx = static_cast<vtkDoubleArray*>(pd->GetArray("sxx"));
    auto sxy = static_cast<vtkDoubleArray*>(pd->GetArray("sxy"));
    auto sxz = static_cast<vtkDoubleArray*>(pd->GetArray("sxz"));
    auto syy = static_cast<vtkDoubleArray*>(pd->GetArray("syy"));
    auto syz = static_cast<vtkDoubleArray*>(pd->GetArray("syz"));
    auto szz = static_cast<vtkDoubleArray*>(pd->GetArray("szz"));
    auto v = static_cast<vtkDoubleArray*>(pd->GetArray("velocity"));
    auto used = static_cast<vtkIntArray*>(pd->GetArray("used"));
    auto bcid = static_cast<vtkIntArray*>(pd->GetArray("borderConditionId"));

    engine.addMaterial(makeMaterialPtr("default", 0, 0, 0));

    Body body("body");

    for (int i = 3; i < argc; i++)
    {
        vector<string> input;
        string s(argv[i]);
        split(input, s, is_any_of(":"));

        string name = input.front();
        string file = input.back();

        LOG_INFO("Loading ani3d mesh " << file << " with name " << name);
        
        TetrMeshFirstOrder mesh;
        mesh.setId(name);
        mesh.setBody(&body);

        reader.readFile(file, &mesh, engine.getDispatcher(), 0, true);

        for (int j = 0; j < mesh.getNodesNumber(); j++)
        {
            CalcNode& node = mesh.getNodeByLocalIndex(j);

            int _i = floor((node.coords.x-bounds[0])/dx);
            int _j = floor((node.coords.y-bounds[2])/dy);
            int _k = floor((node.coords.z-bounds[4])/dz);

            assert_ge(_i, 0);
            assert_ge(_j, 0);
            assert_ge(_k, 0);

            assert_lt(_i, dims[0]);
            assert_lt(_j, dims[1]);
            assert_lt(_k, dims[2]);

            int c = 0;
            int _bcid = 0;
            double vel[3];

            for (int q = 0; q <= 1; q++)
                for (int r = 0; r <= 1; r++)
                    for (int s = 0; s <= 1; s++)
                    {
                        int idx = getIndex(_i+q, _j+r, _k+s);
                        int u = used->GetValue(idx);
                        if (u)
                        {
                            c++;
                            v->GetTupleValue(idx, vel);
                            node.vx += vel[0];
                            node.vy += vel[1];
                            node.vz += vel[2];
                            node.sxx += sxx->GetValue(idx);
                            node.sxy += sxy->GetValue(idx);
                            node.sxz += sxz->GetValue(idx);
                            node.syy += syy->GetValue(idx);
                            node.syz += syz->GetValue(idx);
                            node.szz += szz->GetValue(idx);
                            auto __bcid = bcid->GetValue(idx);
                            if (__bcid > _bcid)
                                _bcid = __bcid;
                        }
                    }

            assert_gt(c, 0);
            node.vx /= c;
            node.vy /= c;
            node.vz /= c;
            node.sxx /= c;
            node.sxy /= c;
            node.sxz /= c;
            node.syy /= c;
            node.syz /= c;
            node.szz /= c;
            node.setBorderConditionId(_bcid);
        }

        mesh.snapshot(snap);
    }

    return 0;
}
