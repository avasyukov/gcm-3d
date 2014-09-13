def options(opt):
    opt.load('compiler_cxx')


def configure(conf):
    conf.load('compiler_cxx')

    conf.start_msg('Checking for boost')

    conf.check_linux()

    conf.env.INCLUDES_LIBBOOST = [
        '/usr/include'
    ]

    conf.env.LIBPATH_LIBBOOST = [
        '/usr/lib',
        '/usr/lib64'
    ]

    conf.env.LIB_LIBBOOST = ['boost_filesystem', 'boost_system']
    try:
        conf.run_c_code(
            code='''
                #include <boost/lexical_cast.hpp>

                int main() {
                    int x = boost::lexical_cast<int>("123");
                    return 0;
                }
            ''',
            compile_filename='libboost_lexical_cast.cpp',
            use='LIBBOOST',
            env=conf.env.derive(),
            define_ret=True,
            features=['cxx', 'cxxprogram', 'test_exec']
        )
        conf.run_c_code(
            code='''
                #include <boost/filesystem.hpp>

                int main() {
                    boost::filesystem::path p(".");
                    p /= "1";
                    return 0;
                }
            ''',
            compile_filename='libboost_bfs.cpp',
            use='LIBBOOST',
            env=conf.env.derive(),
            define_ret=True,
            features=['cxx', 'cxxprogram', 'test_exec']
        )
        version = conf.run_c_code(
            code='''
                #include <boost/version.hpp>
                #include <iostream>

                int main() {
                    std::cout << BOOST_VERSION / 100000 << "." << BOOST_VERSION / 100 % 1000 << "." << BOOST_VERSION % 100;
                    return 0;
                }
            ''',
            compile_filename='libboost_version.cpp',
            use='LIBBOOST',
            env=conf.env.derive(),
            define_ret=True,
            features=['cxx', 'cxxprogram', 'test_exec']
        )
    except:
        conf.end_msg('not found')
        conf.fatal('Library boost not found')
    conf.end_msg(version)

