def options(opt):
    opt.load('compiler_cxx')


def configure(conf):
    conf.load('compiler_cxx')

    conf.start_msg('Checking for libxml2')

    conf.check_linux()

    conf.env.INCLUDES_LIBXML2 = [
        '/usr/include/libxml2'
    ]

    conf.env.LIBPATH_LIBXML2 = [
        '/usr/lib',
        '/usr/lib64',	
        '/usr/lib/x86_64-linux-gnu'

    ]

    LIBS = [
        ['xml2']
    ]

    for libs in LIBS:
        conf.env.LIB_LIBXML2 = libs
        try:
            version = conf.run_c_code(
                code='''
                    #include <stdio.h>
                    #include <libxml/xmlversion.h>
                    int main() {
                        printf(LIBXML_DOTTED_VERSION);
                        return 0;
                    }
                ''',
                use='LIBXML2',
                compile_filename='libxml2test.cpp',
                env=conf.env.derive(),
                define_ret=True,
                features=['cxx', 'cxxprogram', 'test_exec']
            )
        except:
            continue
        conf.end_msg(version)
        return
    conf.end_msg('not found')
    conf.fatal('Library xml2 not found')
