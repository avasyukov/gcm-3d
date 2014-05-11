#include <iostream>
#include <string>

#include "libgcm/config.hpp"

#if CONFIG_ENABLE_LOGGING
#include <log4cxx/propertyconfigurator.h>
#include "launcher/util/FileFolderLookupService.hpp"
#endif

#include "tests/perf/util.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/rheology/Material.hpp"
#include "libgcm/Engine.hpp"
#include "libgcm/rheology/decomposers/AnalyticalRheologyMatrixDecomposer.hpp"
#include "libgcm/rheology/decomposers/NumericalRheologyMatrixDecomposer.hpp"
#include "libgcm/rheology/setters/AnisotropicRheologyMatrixSetter.hpp"
#include "libgcm/util/matrixes.hpp"

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
    
    Material::RheologyProperties C;
    gcm_real la = 1e8;
    gcm_real mu = 1e6;
    C.c12 = C.c13 = C.c23 = la;
    C.c44 = C.c55 = C.c66 = mu;
    C.c11 = C.c22 = C.c33 = la + 2 * mu;
    C.c14 = C.c15 = C.c16 = C.c24 = C.c25 = C.c26 = 1e3;
    C.c34 = C.c35 = C.c36 = C.c45 = C.c46 = C.c56 = 2e3;

    float rho = 1e3;
    gcm_real crackThreshold = numeric_limits<gcm_real>::infinity();

    auto mat = makeMaterialPtr("test", rho, crackThreshold, C);


    gcm_matrix ax, ay, az, u, l, u1;
    AnisotropicRheologyMatrixSetter setter;
    NumericalRheologyMatrixDecomposer numericalDecomposer;
    AnalyticalRheologyMatrixDecomposer analyticalDecomposer;

    CalcNode node;

    setter.setX(ax, mat, node);
    setter.setY(ay, mat, node);
    setter.setZ(az, mat, node);


    auto t1 = measure_time(
        [&]()
        {
            for (int i = 0; i < ITERATIONS; i++) {
                analyticalDecomposer.decomposeX(ax, u, l, u1);                  
                analyticalDecomposer.decomposeY(ay, u, l, u1);                  
                analyticalDecomposer.decomposeZ(az, u, l, u1);                  
            }
        }
    );

    auto t2 = measure_time(
        [&]()
        {
            for (int i = 0; i < ITERATIONS; i++) {
                numericalDecomposer.decomposeX(ax, u, l, u1);                  
                numericalDecomposer.decomposeY(ay, u, l, u1);                  
                numericalDecomposer.decomposeZ(az, u, l, u1);                  
            }
        }
    );

    print_test_results("Analytical decomposer", t1, "Numerical decomposer", t2);

    return 0;
}
