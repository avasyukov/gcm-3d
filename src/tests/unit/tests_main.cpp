#include <gtest/gtest.h>
#include <mpi.h>

#include "libgcm/config.hpp"
#include <gmsh/Gmsh.h>

#if CONFIG_ENABLE_LOGGING
#include <log4cxx/propertyconfigurator.h>
#include "launcher/util/FileFolderLookupService.hpp"
#endif

int main(int argc, char **argv) {
    MPI::Init();
    GmshInitialize();
    #if CONFIG_ENABLE_LOGGING
    auto& ffls = launcher::FileFolderLookupService::getInstance();
    ffls.addPath("src/tests");
    log4cxx::PropertyConfigurator::configure(ffls.lookupFile("log4cxx.properties"));
    #endif
    testing::InitGoogleTest(&argc, argv);
    int res = RUN_ALL_TESTS();
    GmshFinalize();
    MPI::Finalize();
    return res;
}
