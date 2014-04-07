#include <iostream>
#include <string>

#include "libgcm/config.hpp"

#if CONFIG_ENABLE_LOGGING
#include <log4cxx/propertyconfigurator.h>
#include "launcher/util/FileFolderLookupService.hpp"
#endif

#include "tests/perf/util.hpp"
#include "libgcm/util/AnisotropicMatrix3D.hpp"
#include "libgcm/util/AnisotropicMatrix3DAnalytical.hpp"
#include "libgcm/materials/AnisotropicElasticMaterial.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/Engine.hpp"

#define ITERATIONS 1000

using namespace gcm;
using namespace std;

int main()
{
    #if CONFIG_ENABLE_LOGGING
    auto& ffls = launcher::FileFolderLookupService::getInstance();
    ffls.addPath("src/tests");
    log4cxx::PropertyConfigurator::configure(ffls.lookupFile("log4cxx.properties"));
    #endif

    cout << "This test compares performance of two anisotropic elastic matrix implementations: numerical and analytical." << endl;
    cout << "Analytical implementation is expected to be few times faster." << endl;
    
    AnisotropicMatrix3DAnalytical *analyticalMatrices = nullptr;
    AnisotropicMatrix3D *numericalMatrices = nullptr;
    CalcNode anisotropicNode;
    

    IAnisotropicElasticMaterial::RheologyParameters C;
    gcm_real la = 1e8;
    gcm_real mu = 1e6;
    C.c12 = C.c13 = C.c23 = la;
    C.c44 = C.c55 = C.c66 = mu;
    C.c11 = C.c22 = C.c33 = la + 2 * mu;
    C.c14 = C.c15 = C.c16 = C.c24 = C.c25 = C.c26 = 1e3;
    C.c34 = C.c35 = C.c36 = C.c45 = C.c46 = C.c56 = 2e3;

    float rho = 1e3;
    gcm_real crackThreshold = numeric_limits<gcm_real>::infinity();

    AnisotropicElasticMaterial mat("AnisotropicMatrix3D_Perf", rho, crackThreshold, C);
    anisotropicNode.setMaterialId(Engine::getInstance().addMaterial(&mat));

    auto t1 = measure_time(
        [&]()
        {
            delete[] analyticalMatrices;
            analyticalMatrices = new AnisotropicMatrix3DAnalytical[ITERATIONS];
        },
        [&]()
        {
            for (int i = 0; i < ITERATIONS; i++) {
                analyticalMatrices[i].createAx(anisotropicNode);
                analyticalMatrices[i].createAy(anisotropicNode);
                analyticalMatrices[i].createAz(anisotropicNode);
            }
        }
    );

    auto t2 = measure_time(
        [&]()
        {
            delete[] numericalMatrices;
            numericalMatrices = new AnisotropicMatrix3D[ITERATIONS];
        },
        [&]()
        {
            for (int i = 0; i < ITERATIONS; i++) {
                numericalMatrices[i].createAx(anisotropicNode);
                numericalMatrices[i].createAy(anisotropicNode);
                numericalMatrices[i].createAz(anisotropicNode);
            }
        }
    );

    print_test_results("AnisotropicMatrix3DAnalytical", t1, "AnisotropicMatrix3D", t2);

    return 0;
}
