def options(opt):
    opt.load('compiler_cxx')


def configure(conf):
    conf.load('compiler_cxx')

    conf.start_msg('Checking for gsl')

    conf.check_linux()

    conf.env.INCLUDES_LIBGSL = [
        '/usr/include'
    ]

    conf.env.LIBPATH_LIBGSL = [
        '/usr/lib',
        '/usr/lib64'
    ]

    LIBS = [
        ['gsl', 'gslcblas', 'm']
    ]

    for libs in LIBS:
        conf.env.LIB_LIBGSL = libs
        try:
            version = conf.run_c_code(
                code='''
                    #include <stdio.h>
                    #include <gsl/gsl_version.h>
                    int main() {
                        printf(GSL_VERSION);
                        return 0;
                    }
                ''',
                use='LIBGSL',
                compile_filename='libgsltest.cpp',
                env=conf.env.derive(),
                define_ret=True,
                features=['cxx', 'cxxprogram', 'test_exec']
            )
        except:
            continue
        conf.end_msg(version)
        return
    conf.end_msg('not found')
    conf.fatal('Library gsl not found')
