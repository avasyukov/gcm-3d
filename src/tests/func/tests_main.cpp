#include <gtest/gtest.h>
#include <mpi.h>

#ifdef CONFIG_ENABLE_LOGGING
#include <log4cxx/propertyconfigurator.h>
#include "libgcm/Utils.h"
#endif

int main(int argc, char **argv) {
    MPI::Init();
    #ifdef CONFIG_ENABLE_LOGGING
    gcm::FileFolderLookupService fls;
    fls.addPath("src/tests");
    log4cxx::PropertyConfigurator::configure(fls.lookupFile("log4cxx.properties"));
    #endif
    testing::InitGoogleTest(&argc, argv);
    int res = RUN_ALL_TESTS();
    MPI::Finalize();
    return res;
}
