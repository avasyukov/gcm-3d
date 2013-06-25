def options(opt):
    opt.load('compiler_cxx')


def configure(conf):
    conf.load('compiler_cxx')

    conf.start_msg('Checking for libxml++')

    conf.check_linux()

    conf.env.INCLUDES_LIBXMLPP = [
        '/usr/include/libxml++-2.6',
        '/usr/include/glibmm-2.4',
        '/usr/lib64/glibmm-2.4/include',
        '/usr/include/glib-2.0',
        '/usr/lib64/glib-2.0/include',
        '/usr/lib64/libxml++-2.6/include'
    ]

    conf.env.LIBPATH_LIBXMLPP = [
        '/usr/lib',
        '/usr/lib64'
    ]

    LIBS = [
        ['xml++-2.6', 'xml2', 'glibmm-2.4', 'gobject-2.0', 'sigc-2.0', 'glib-2.0']
    ]

    for libs in LIBS:
        conf.env.LIB_LIBXMLPP = libs
        try:
            conf.run_c_code(
                code='''
                    #include <iostream>
                    #include <libxml++/libxml++.h>
                    using namespace std;
                    using namespace xmlpp;
                    int main() {
                        DomParser p;
                        p.parse_memory("<a><b></b><b></b></a>");
                        Element* re = p.get_document()->get_root_node();
                        cout << re->find("/a/b").size() << endl;
                        return 0;
                    }
                ''',
                use='LIBXMLPP',
                compile_filename='libxmlpptest.cpp',
                env=conf.env.derive(),
                define_ret=True,
                features=['cxx', 'cxxprogram', 'test_exec']
            )
        except:
            continue
        conf.end_msg('found')
        return
    conf.end_msg('not found')
    conf.fatal('Library xml++ not found')
