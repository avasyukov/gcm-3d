def options(opt):
    opt.load('compiler_cxx')


def configure(conf):
    conf.load('compiler_cxx')

    conf.start_msg('Checking for openmpi version')

    conf.check_linux()

    conf.env.INCLUDES_LIBOPENMPI = [
        '/usr/include/openmpi',
        '/usr/include/openmpi-x86_64'
    ]

    conf.env.LIBPATH_LIBOPENMPI = [
        '/usr/lib/openmpi/lib',
        '/usr/lib64/openmpi/lib'
    ]

    LIBS = [
        ['mpi', 'mpi_cxx']
    ]

    for libs in LIBS:
        conf.env.LIB_LIBOPENMPI = libs
        try:
            version = conf.run_c_code(
                code='''
                    #include <stdio.h>
                    #include <mpi.h>
                    int main() {
                        printf("%d.%d.%d", OMPI_MAJOR_VERSION, OMPI_MINOR_VERSION, OMPI_RELEASE_VERSION);
                        return 0;
                    }
                ''',
                use='LIBOPENMPI',
                compile_filename='libopenmpitest.cpp',
                env=conf.env.derive(),
                define_ret=True,
                features=['cxx', 'cxxprogram', 'test_exec']
            )
        except:
            continue
        conf.end_msg(version)
        return
    conf.end_msg('not found')
    conf.fatal('Library openmpi not found')
