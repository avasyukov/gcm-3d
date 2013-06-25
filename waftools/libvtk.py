def options(opt):
    opt.load('compiler_cxx')


def configure(conf):
    conf.load('compiler_cxx')

    conf.start_msg('Checking for vtk version')

    conf.check_linux()

    conf.env.INCLUDES_LIBVTK = [
        '/usr/include/vtk'
    ]

    conf.env.LIBPATH_LIBVTK = [
        '/usr/lib',
        '/usr/lib64',
        '/usr/lib64/vtk'
    ]

    LIBS = [
        ['vtkCommon', 'vtkFiltering', 'vtkIO']
    ]

    for libs in LIBS:
        conf.env.LIB_LIBVTK = libs
        try:
            version = conf.run_c_code(
                code='''
                    #include <stdio.h>
                    #include <vtkVersion.h>
                    int main() {
                        printf("%s", vtkVersion::GetVTKVersion() );
                        return 0;
                    }
                ''',
                use='LIBVTK',
                compile_filename='libvtktest.cpp',
                env=conf.env.derive(),
                define_ret=True,
                features=['cxx', 'cxxprogram', 'test_exec']
            )
        except:
            continue
        conf.end_msg(version)
        return
    conf.end_msg('not found')
    conf.fatal('Library vtk not found')

