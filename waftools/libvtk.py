import os

def options(opt):
    opt.load('compiler_cxx')


def configure(conf):
    conf.load('compiler_cxx')

    conf.start_msg('Checking for vtk')

    conf.check_linux()

    conf.env.INCLUDES_LIBVTK = [
        '/usr/include/vtk',
    ]

    conf.env.LIBPATH_LIBVTK = [
        '/usr/lib',
        '/usr/lib64',
        '/usr/lib64/vtk',
    ]

    include_path = '/usr/include'
    for d in os.listdir(include_path):
        full_path = include_path + '/' + d
        if os.path.isdir(full_path):
            if d.startswith('vtk-'):
                conf.env.INCLUDES_LIBVTK += [full_path]
    
    lib_path = [
        '/usr/lib64/',
        '/usr/lib'
    ]
    for l in  lib_path:
        for d in os.listdir(l):
            full_path = l + '/' + d
            if os.path.isdir(full_path):
                if d.startswith('vtk-'):
                    conf.env.LIBPATH_LIBVTK += [full_path]

    LIBS = {
        '5': ['vtkCommon', 'vtkFiltering', 'vtkIO'],
        '6': ['vtkCommonCore', 'vtkCommonDataModel', 'vtkFiltersCore', 'vtkIOCore', 'vtkIOXML']
    }

    for v in LIBS:
        libs = LIBS[v]
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
        conf.define('CONFIG_VTK_%s' % v, 1)
        conf.end_msg(version)
        return
    conf.end_msg('not found')
    conf.fatal('Library vtk not found')

