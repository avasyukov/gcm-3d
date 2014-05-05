import os

def options(opt):
    opt.load('compiler_cxx')


def configure(conf):
    conf.load('compiler_cxx')

    conf.start_msg('Checking for vtk')

    conf.check_linux()

    conf.env.INCLUDES_LIBVTK = [
        '/usr/include/vtk-5.8'
    ]

    conf.env.LIBPATH_LIBVTK = [
        '/usr/lib',
        '/usr/lib/vtk-5.8'
    ]

    include_path = '/usr/include/vtk-5.8'
    for d in os.listdir(include_path):
        full_path = include_path + '/' + d
        if os.path.isdir(full_path):
            if d.startswith('vtk-'):
                conf.env.INCLUDES_LIBVTK += [full_path]
    
    lib_path = [
        '/usr/lib',
        '/usr/lib/vtk-5.8'
    ]
    for l in  lib_path:
        for d in os.listdir(l):
            full_path = l + '/' + d
            if os.path.isdir(full_path):
                if d.startswith('vtk-'):
                    conf.env.LIBPATH_LIBVTK += [full_path]

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

