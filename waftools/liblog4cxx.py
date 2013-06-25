def options(opt):
    opt.load('compiler_cxx')


def configure(conf):
    conf.load('compiler_cxx')

    conf.start_msg('Checking for log4cxx')

    conf.check_linux()

    conf.env.INCLUDES_LIBLOG4CXX = [
        '/usr/include'
    ]

    conf.env.LIBPATH_LIBLOG4CXX = [
        '/usr/lib',
        '/usr/lib64'
    ]

    LIBS = [
        ['log4cxx']
    ]

    for libs in LIBS:
        conf.env.LIB_LIBLOG4CXX = libs
        try:
            conf.run_c_code(
                code='''
                    #include "log4cxx/logger.h"
                    #include "log4cxx/basicconfigurator.h"

                    using namespace log4cxx;

                    int main(int argc, char **argv)
                    {
                        BasicConfigurator::configure();
                        LoggerPtr logger(Logger::getLogger("test"));
                        LOG4CXX_INFO(logger, "INFO");
                        return 0;
                    }''',
                use='LIBLOG4CXX',
                compile_filename='log4cxxtest.cpp',
                env=conf.env.derive(),
                define_ret=True,
                features=['cxx', 'cxxprogram', 'test_exec']
            )
        except:
            continue
        conf.end_msg('found')
        return
    conf.end_msg('not found')
    conf.fatal('Library log4cxx not found')
