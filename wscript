import sys
import os

from waflib import ConfigSet
from waflib.Build import BuildContext
from waflib.Build import CleanContext
from waflib.Build import InstallContext
from waflib.Build import UninstallContext
from waflib.Task import Task
from waflib.Task import always_run
from waflib.Configure import ConfigurationContext


VERSION = '0.1'
APPNAME = 'gcm3d'

out = 'build'
env_file = os.path.join(out, 'current_env.txt')


def init(ctx):
    env = ConfigSet.ConfigSet()

    try:
        env.load(env_file)
    except:
        return
    build_variant = env.variant

    for c in (BuildContext, CleanContext, InstallContext, UninstallContext, ConfigurationContext):
        c.variant = build_variant


def options(opt):
    '''Sets gcm3d specific options'''

    pcog = opt.add_option_group('Project components options')

    pcog.add_option(
        '--without-launcher',
        action='store_true',
        default=False,
        help='Disable gcm3d launcher'
    )

    pcog.add_option(
        '--without-logging',
        action='store_true',
        default=False,
        help='Disable libgcm logging routines'
    )

    pcog.add_option(
        '--without-tests',
        action='store_true',
        default=False,
        help='Do not build tests'
    )

    pcog.add_option(
        '--with-headers',
        action='store_true',
        default=False,
        help='Install header files'
    )

    pcog.add_option(
        '--with-resources',
        action='store_true',
        default=False,
        help='Install resource files'
    )

    pbog = opt.add_option_group('Project build options')

    pbog.add_option(
        '--disable-auto-rpath',
        action='store_true',
        default=False,
        help='Do not adjust linker rpath automatically'
    )

    pbog.add_option(
        '--static',
        action='store_true',
        default=False,
        help='Build static library instead of dynamic one'
    )

    pbog.add_option(
        '--debug-symbols',
        action='store_true',
        default=False,
        help='Add debug symbols to binary'
    )

    pbog.add_option(
        '--optimize',
        action='store_true',
        default=False,
        help='Use compiler optimization flags'
    )

    pbog.add_option(
        '--profile',
        action='store_true',
        default=False,
        help='Add profiling support'
    )

    pbog.add_option(
        '--coverage',
        action='store_true',
        default=False,
        help='Add test coverage support'
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
    conf.msg('Install headers', yes_no(conf.options.with_headers))
    conf.msg('Install resources', yes_no(conf.options.with_resources))
    conf.msg('Add debug symbols', yes_no(conf.options.debug_symbols))
    conf.msg('Use optimizations', yes_no(conf.options.optimize))
    conf.msg('Add profiling support', yes_no(conf.options.profile))
    conf.msg('Add test coverage support', yes_no(conf.options.coverage))

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
    conf.env.with_headers = conf.options.with_headers
    conf.env.with_resources = conf.options.with_resources
    conf.env.static = conf.options.static

    conf.env.CXXFLAGS += ['-Wall']
    conf.env.CXXFLAGS += ['-Wno-deprecated']
    conf.env.CXXFLAGS += ['-std=c++11']

    conf.env.CXXFLAGS += ['-DCONFIG_PREFIX="%s"' % conf.options.prefix]
    if conf.env.with_resources:
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

    build_variant = []

    if conf.options.debug_symbols:
        conf.env.CXXFLAGS += ['-g']
        build_variant += ['debug']

    if conf.options.optimize:
        conf.env.CXXFLAGS += ['-O3', '-funroll-loops']
        build_variant += ['optimize']

    if conf.options.profile:
        conf.env.CXXFLAGS += ['-pg']
        build_variant += ['profile']

    if conf.options.coverage:
        conf.env.CXXFLAGS += ['-fprofile-arcs', '-ftest-coverage']
        conf.env.LINKFLAGS += ['-fprofile-arcs']
        build_variant += ['coverage']

    if len(build_variant) == 0:
        build_variant += ['default']

    conf.env.variant = '_'.join(build_variant)

    conf.load(libs, tooldir='waftools')
    conf.env.LIBS = libs

    if not conf.options.disable_auto_rpath:
        keys = [x for x in conf.env.keys() if x.find('LIBPATH_LIB') >= 0]
        for key in keys:
            conf.env.LINKFLAGS += ['-Wl,-rpath,' + x for x in conf.env[key]]
        if conf.options.prefix:
            conf.env.LINKFLAGS += ['-Wl,-rpath,' + conf.options.prefix]

        conf.env.LINKFLAGS = list(set(conf.env.LINKFLAGS))
        conf.env.LINKFLAGS += ['-Wl,-rpath,%s/lib' % os.path.abspath(conf.options.prefix)]

    conf.env.store(env_file)


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

    if bld.env.with_resources:
        bld.install_files(
            '${PREFIX}/share/doc/%s' % APPNAME,
            ['README']
        )
        bld.install_files(
            '${PREFIX}/share/%s' % APPNAME,
            ['src/launcher/log4cxx.properties']
        )
        bld.install_files(
            '${PREFIX}/share/%s/models' % APPNAME,
            bld.path.ant_glob('models/*')
        )
        bld.install_files(
            '${PREFIX}/share/%s/tasks' % APPNAME,
            bld.path.ant_glob('tasks/*')
        )

    if bld.env.with_headers:
        bld.install_files(
            '${PREFIX}/include/%s-%s/%s' % (APPNAME, VERSION, APPNAME),
            bld.path.ant_glob('**/*.h'),
            cwd=src_dir,
            relative_trick=True
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
