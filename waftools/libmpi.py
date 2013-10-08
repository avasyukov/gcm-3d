import subprocess

def options(opt):
    opt.load('compiler_cxx')


def configure(conf):
    conf.load('compiler_cxx')

    conf.check_linux()

    conf.env.CFLAGS_LIBMPI = [
    ]

    conf.env.INCLUDES_LIBMPI = [
    ]

    conf.env.LIBPATH_LIBMPI = [
    ]
    
    conf.env.LIB_LIBMPI = [
    ]

    conf.env.LINKFLAGS_LIBMPI = [
    ]
    
    conf.start_msg('Extracting MPI comiler and linker flags')
    try:
        output = subprocess.Popen(['mpicxx', '-show'], stdout=subprocess.PIPE).communicate()[0].decode('utf-8')
        params = output.split('\n')[0].split(' ')[1:]

        unprocessed = []
        for x in params:
            if x.startswith('-I'):
                conf.env.INCLUDES_LIBMPI += [x[2:]]
            elif x.startswith('-p'):
                conf.env.CFLAGS_LIBMPI += [x]
            elif x.startswith('-m'):
                conf.env.CFLAGS_LIBMPI += [x]
            elif x.startswith('-L'):
                conf.env.LIBPATH_LIBMPI += [x[2:]]
                conf.env.LINKFLAGS_LIBMPI += ['-Wl,-rpath,' + x[2:]]
            elif x.startswith('-l'):
                conf.env.LIB_LIBMPI += [x[2:]]
            elif x.startswith('-Wl'):
                conf.env.LINKFLAGS_LIBMPI += [x]
            else:
                unprocessed += [x]
        if unprocessed:
           conf.end_msg('done (unprocessed parameters: %s)' % unprocessed)
        else:
           conf.end_msg('done')
    except:
        raise
        conf.end_msg('failed')
        conf.fatal('Cannot extract MPI compiler and linker flags')

    try:
        conf.start_msg('Checking for mpi version')
        version = conf.run_c_code(
            code='''
                #include <stdio.h>
                #include <mpi.h>
                int main() {
                    printf("%d.%d", MPI_VERSION, MPI_SUBVERSION);
                    return 0;
                }
            ''',
            use='LIBMPI',
            compile_filename='libmpitest.cpp',
            env=conf.env.derive(),
            define_ret=True,
            features=['cxx', 'cxxprogram', 'test_exec']
        )
        conf.end_msg(version)
    except:
        conf.end_msg('not found')
        conf.fatal('Library mpi cannot be found. Please check that you\'re using an appropriate compiler.')
        return
    
