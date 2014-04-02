#include <gtest/gtest.h>
#include <mpi.h>

#ifdef CONFIG_ENABLE_LOGGING
#include <log4cxx/propertyconfigurator.h>
#include "libgcm/Utils.hpp"
#include "launcher/util/FileFolderLookupService.hpp"
#endif

int main(int argc, char **argv) {
    MPI::Init();
    #ifdef CONFIG_ENABLE_LOGGING
    auto& ffls = launcher::FileFolderLookupService::getInstance();
    ffls.addPath("src/tests");
    log4cxx::PropertyConfigurator::configure(ffls.lookupFile("log4cxx.properties"));
    #endif
    testing::InitGoogleTest(&argc, argv);
    int res = RUN_ALL_TESTS();
    MPI::Finalize();
    return res;
}
