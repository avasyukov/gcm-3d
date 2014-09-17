def options(opt):
    opt.load('compiler_cxx')


def configure(conf):
    conf.load('compiler_cxx')

    conf.start_msg('Checking for boost::mpi')

    conf.check_linux()


    conf.env.INCLUDES_LIBBOOST_MPI = []
    conf.env.LIBPATH_LIBBOOST_MPI = []
    conf.env.LINKFLAGS_LIBBOOST_MPI = []

    conf.env.INCLUDES_LIBBOOST_MPI.extend(conf.env.INCLUDES_LIBMPI)
    conf.env.INCLUDES_LIBBOOST_MPI.extend([
        '/usr/include'
    ])

    conf.env.LIBPATH_LIBBOOST_MPI.extend(conf.env.LIBPATH_LIBMPI)
    conf.env.LIBPATH_LIBBOOST_MPI.extend([
        '/usr/lib',
        '/usr/lib64',
    ])
    
    conf.env.LINKFLAGS_LIBBOOST_MPI.extend(conf.env.LINKFLAGS_LIBMPI)
    
    LIBS = [
        ['boost_mpi', 'boost_serialization'],
        ['boost_mpi-mt', 'boost_serialization']
    ]

    for libs in LIBS:
        conf.env.LIB_LIBBOOST_MPI = []
        conf.env.LIB_LIBBOOST_MPI.extend(conf.env.LIB_LIBMPI)
        conf.env.LIB_LIBBOOST_MPI.extend(libs)
        try:
            conf.run_c_code(
                code='''
                    #include <boost/mpi.hpp>

                    int main() {
                        boost::mpi::environment env;
                        boost::mpi::communicator world;
                        return 0;
                    }
                ''',
                compile_filename='libboost_mpi.cpp',
                use='LIBBOOST_MPI',
                env=conf.env.derive(),
                define_ret=True,
                features=['cxx', 'cxxprogram', 'test_exec']
            )
        except:
            continue
        conf.end_msg('found')
        return
    conf.end_msg('not found')
    conf.fatal('Library boost::mpi not found')
