def options(opt):
    opt.load('compiler_cxx')


def configure(conf):
    conf.load('compiler_cxx')

    conf.start_msg('Checking for mpich2 version')

    conf.check_linux()

    conf.env.INCLUDES_LIBMPICH2 = [
        '/usr/include/mpich2-x86_64'
    ]

    conf.env.LIBPATH_LIBMPICH2 = [
        '/usr/lib64/mpich2/lib'
    ]

    conf.env.LINKFLAGS_LIBMPICH2 = [
        '-Wl,-z,noexecstack'
    ]

    conf.env.CXXFLAGS_LIBMPICH2 = [
        '-fPIC'
    ]

    LIBS = [
        ['mpich', 'opa', 'mpl', 'rt', 'pthread', 'mpichcxx']
    ]

    for libs in LIBS:
        conf.env.LIB_LIBMPICH2 = libs
        try:
            version = conf.run_c_code(
                code='''
                    #include <stdio.h>
                    #include <mpi.h>
                    int main() {
                        printf("%s", MPICH2_VERSION);
                        return 0;
                    }
                ''',
                use='LIBMPICH2',
                compile_filename='libmpich2test.cpp',
                define_ret=True,
                env=conf.env.derive(),
                features=['cxx', 'cxxprogram', 'test_exec']
            )
        except:
            continue
        conf.end_msg(version)
        return
    conf.end_msg('not found')
    conf.fatal('Library mpich2 not found')
