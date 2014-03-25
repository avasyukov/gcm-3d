import sys
import os
from waflib.Build import BuildContext
from waflib.Task import Task
from waflib.Task import always_run


VERSION = '0.1'
APPNAME = 'gcm3d'


def options(opt):
    '''Sets gcm3d specific options'''

    opt.add_option(
        '--without-launcher',
        action='store_true',
        default=False,
        help='Disable gcm3d launcher'
    )

    opt.add_option(
        '--without-logging',
        action='store_true',
        default=False,
        help='Disable libgcm logging routines'
    )

    opt.add_option(
        '--without-tests',
        action='store_true',
        default=False,
        help='Do not execute tests'
    )

    opt.add_option(
        '--without-default-cxxflags',
        action='store_true',
        default=False,
        help='Disable default cxxflags'
    )

    opt.add_option(
        '--without-headers',
        action='store_true',
        default=False,
        help='Do not install header files'
    )

    opt.add_option(
        '--without-resources',
        action='store_true',
        default=False,
        help='Do not install resources files'
    )
    
    opt.add_option(
        '--disable-auto-rpath',
        action='store_true',
        default=False,
        help='Do not adjust linker rpath automatically'
    )

    opt.add_option(
        '--static',
        action='store_true',
        default=False,
        help='Build static library instead of dynamic one'
    )

    opt.load('compiler_cxx')
    opt.load('utils', tooldir='waftools')


def configure(conf):
    '''Configures build environment'''

    def yes_no(b):
        return 'yes' if b else 'no'
    
    conf.msg('Prefix', conf.options.prefix)
    conf.msg('Build static lib', yes_no(conf.options.static))
    conf.msg('Build launcher', yes_no(not conf.options.without_launcher))
    conf.msg('Enable logging', yes_no(not conf.options.without_logging))
    conf.msg('Execute tests', yes_no(not conf.options.without_tests))
    conf.msg('Install headers', yes_no(not conf.options.without_headers))
    conf.msg('Install resources', yes_no(not conf.options.without_resources))

    libs = [
        'utils',
        'libmpi',
        'libgsl',
        'libxml2',
        'libgmsh',
        'libvtk'
    ]

    conf.env.without_launcher = conf.options.without_launcher
    conf.env.without_logging = conf.options.without_logging
    conf.env.without_tests = conf.options.without_tests
    conf.env.without_headers = conf.options.without_headers
    conf.env.without_resources = conf.options.without_resources
    conf.env.static = conf.options.static

    if not conf.options.without_default_cxxflags:
        conf.env.CXXFLAGS += ['-Wall']
        conf.env.CXXFLAGS += ['-Wno-deprecated']
        conf.env.CXXFLAGS += ['-std=c++11']

    conf.env.CXXFLAGS += ['-DCONFIG_PREFIX="%s"' % conf.options.prefix]
    if not conf.env.without_resources:
        conf.env.CXXFLAGS += ['-DCONFIG_SHARE_GCM="%s/share/gcm3d"' % conf.options.prefix]
    else:
        conf.env.CXXFLAGS += ['-DCONFIG_SHARE_GCM="."']

    if not conf.env.without_logging:
        conf.env.CXXFLAGS += ['-DCONFIG_ENABLE_LOGGING']
        libs.append('liblog4cxx')

    if not conf.env.without_tests:
        libs.append('libgtest')
    
    if not conf.env.without_tests or not conf.env.without_launcher:
        libs.append('libboost')

    conf.env.LINKFLAGS += ['-Wl,-rpath,%s/lib' % os.path.abspath(conf.options.prefix)]

    conf.load(libs, tooldir='waftools')
    conf.env.LIBS = libs
 
    if not conf.options.disable_auto_rpath:
        keys = [x for x in conf.env.keys() if x.find('LIBPATH_LIB') >= 0]
        for key in keys:
            conf.env.LINKFLAGS += ['-Wl,-rpath,' + x for x in conf.env[key]]
        if conf.options.prefix:
            conf.env.LINKFLAGS += ['-Wl,-rpath,' + conf.options.prefix]

        conf.env.LINKFLAGS = list(set(conf.env.LINKFLAGS))


def build(bld):
    '''Builds project'''

    bld.load(bld.env.LIBS, tooldir='waftools')

    libs = [l.upper() for l in bld.env.LIBS]

    src_dir = bld.path.find_dir('src/libgcm')

    if bld.env.static:
        lib_type = 'cxxstlib'
    else:
        lib_type = 'cxxshlib'

    bld(
        features='cxx %s' % lib_type,
        source=bld.path.ant_glob('src/libgcm/**/*.cpp'),
        use=libs,
        name='gcm',
        target='gcm'
    )

    if not bld.env.without_launcher:
        bld(
            features='cxx cxxprogram',
            source=bld.path.ant_glob('src/launcher/**/*.cpp'),
            includes=['src'],
            use=['gcm'] + libs,
            target='gcm3d'
        )

    if not bld.env.without_tests:
        bld(
            features='cxx cxxprogram',
            source=bld.path.ant_glob('src/tests/unit/**/*.cpp') + 
                bld.path.ant_glob('src/launcher/loaders/**/*.cpp') + [
                bld.path.find_node('src/launcher/util/xml.cpp')
            ],
            includes=['src'],
            use=['gcm'] + libs,
            target='gcm3d_unit_tests',
            install_path=None
        )
        bld(
            features='cxx cxxprogram',
            source=bld.path.ant_glob('src/tests/func/**/*.cpp') + 
                bld.path.ant_glob('src/launcher/loaders/**/*.cpp') + [
                bld.path.find_node('src/launcher/launcher.cpp'),
                bld.path.find_node('src/launcher/util/xml.cpp')
            ],
            includes=['src'],
            use=['gcm'] + libs,
            target='gcm3d_func_tests',
            install_path=None
        )
        perf_test_sources = [x for x in bld.path.ant_glob('src/tests/perf/*.cpp') if x.name != 'util.cpp']
        for s in perf_test_sources:
            bld(
                features='cxx cxxprogram',
                source=[
                    bld.path.find_node('src/tests/perf/util.cpp'),
                    s
                ],
                includes='src',
                lib=['rt'],
                use=['gcm'] + libs,
                target='gcm3d_perf_%s' % s.name[:-4],
                install_path=None
            )

    if not bld.env.without_resources:
        bld.install_files(
            '${PREFIX}/share/doc/%s' % APPNAME,
            ['README']
        )

    if not bld.env.without_resources:
        bld.install_files(
            '${PREFIX}/share/%s' % APPNAME,
            ['src/launcher/log4cxx.properties']
        )

    if not bld.env.without_headers:
        bld.install_files(
            '${PREFIX}/include/%s-%s/%s' % (APPNAME, VERSION, APPNAME),
            bld.path.ant_glob('**/*.h'),
            cwd=src_dir,
            relative_trick=True
        )

    if not bld.env.without_resources:
        bld.install_files(
            '${PREFIX}/share/%s/models' % APPNAME,
            bld.path.ant_glob('models/*')
        )
        bld.install_files(
            '${PREFIX}/share/%s/tasks' % APPNAME,
            bld.path.ant_glob('tasks/*')
        )


class func_tests_target(BuildContext):
    '''run gcm3d functional tests'''
    cmd = 'func-tests'
    fun = '__run_tests'


class unit_tests_target(BuildContext):
    '''run gcm3d functional tests'''
    cmd = 'unit-tests'
    fun = '__run_tests'


@always_run
class TestRunner(Task):
    def __init__(self, cwd, *args, **kwargs):
        self.cwd = cwd
        super(TestRunner, self).__init__(*args, **kwargs)

    def run(self):
        self.exec_command(self.inputs[0].abspath(), stdout=sys.stdout, stderr=sys.stderr, cwd=self.cwd)


def __run_tests(ctx):
    if ctx.env.without_tests:
        ctx.fatal('Project was configured without testing support')
    build(ctx)
    r = TestRunner(
        ctx.path.abspath(),
        env=ctx.env
    )
    r.set_inputs([ctx.path.find_or_declare('gcm3d_unit_tests' if ctx.cmd == 'unit-tests' else 'gcm3d_func_tests')]),
    ctx.add_to_group(r)
