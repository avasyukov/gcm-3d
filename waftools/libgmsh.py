def options(opt):
    opt.load('compiler_cxx')


def configure(conf):
    conf.load('compiler_cxx')

    conf.start_msg('Checking for gmsh version')

    conf.check_linux()

    conf.env.INCLUDES_LIBGMSH = [
        '/usr/include/gmsh',
        '/usr/local/include/gmsh'
    ]

    conf.env.LIBPATH_LIBGMSH = [
        '/usr/lib',
        '/usr/lib64',
		'/usr/local/lib'
    ]

    LIBS = [
        ['gmsh'],
        ['Gmsh']
    ]

    for libs in LIBS:
        conf.env.LIB_LIBGMSH = libs
        try:
            version = conf.run_c_code(
                code='''
                    #include <stdio.h>
                    #include <GmshVersion.h>
                    int main() {
                        printf(GMSH_VERSION);
                        return 0;
                    }
                ''',
                use='LIBGMSH',
                compile_filename='libgmshtest.cpp',
                env=conf.env.derive(),
                define_ret=True,
                features=['cxx', 'cxxprogram', 'test_exec']
            )
        except:
            continue
        conf.end_msg(version)
        return
    conf.end_msg('not found')
    conf.fatal('Gmsh not found')
