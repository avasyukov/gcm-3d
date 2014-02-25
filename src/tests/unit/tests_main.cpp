#include <gtest/gtest.h>

#ifdef CONFIG_ENABLE_LOGGING
#include <log4cxx/propertyconfigurator.h>
#include "Utils.h"
#endif

int main(int argc, char **argv) {
	#ifdef CONFIG_ENABLE_LOGGING
	gcm::FileLookupService fls;
	fls.addPath("src/tests");
	log4cxx::PropertyConfigurator::configure(fls.lookupFile("log4cxx.properties"));
	#endif
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
