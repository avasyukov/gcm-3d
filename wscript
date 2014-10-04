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
from waflib.Configure import conf


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
        '--logging-level',
        action='store',
        default='trace',
        help='Disable libgcm logging routines'
    )
    
    pcog.add_option(
        '--disable-assertions',
        action='store_true',
        default=False,
        help='Disable libgcm assert routines'
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
        '--no-export-dynamic-symbols',
        action='store_true',
        default=False,
        help='Do not pass --export-dynamic option to linker'
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


@conf
def check_linux(conf):
    if not sys.platform.startswith('linux'):
        conf.fatal('Only Linux platform is supported at the moment')

def configure(conf):
    '''Configures build environment'''

    def yes_no(b):
        return 'yes' if b else 'no'

    logging_levels = ['none', 'fatal', 'error', 'warn', 'info', 'debug', 'trace'] 
    if not conf.options.logging_level in logging_levels:
        conf.fatal('Unknown logging level specified. Valid levels are ' + ', '.join(logging_levels))

    conf.msg('Prefix', conf.options.prefix)
    conf.msg('Build static lib', yes_no(conf.options.static))
    conf.msg('Build launcher', yes_no(not conf.options.without_launcher))
    conf.msg('Logging level', conf.options.logging_level)
    conf.msg('Enable assertions', yes_no(not conf.options.disable_assertions))
    conf.msg('Execute tests', yes_no(not conf.options.without_tests))
    conf.msg('Install headers', yes_no(conf.options.with_headers))
    conf.msg('Install resources', yes_no(conf.options.with_resources))
    conf.msg('Add debug symbols', yes_no(conf.options.debug_symbols))
    conf.msg('Use optimizations', yes_no(conf.options.optimize))
    conf.msg('Add profiling support', yes_no(conf.options.profile))
    conf.msg('Add test coverage support', yes_no(conf.options.coverage))

    libs = [
        'libmpi',
        'libgsl',
        'libxml2',
        'libgmsh',
        'libvtk'
    ]

    conf.env.without_launcher = conf.options.without_launcher
    conf.env.logging_level = conf.options.logging_level
    conf.env.disable_assertions = conf.options.disable_assertions
    conf.env.without_tests = conf.options.without_tests
    conf.env.with_headers = conf.options.with_headers
    conf.env.with_resources = conf.options.with_resources
    conf.env.no_export_dynamic_symbols = conf.options.no_export_dynamic_symbols
    conf.env.static = conf.options.static

    conf.env.CXXFLAGS += ['-Wall']
    conf.env.CXXFLAGS += ['-std=c++11']

    conf.env.LINKFLAGS += ['-lpthread', '-lrt', '-lstdc++']
    if not conf.env.no_export_dynamic_symbols:
        conf.env.LINKFLAGS += ['-rdynamic']

    conf.env.INCLUDES += [conf.path.find_dir('src').abspath()]

    conf.define('CONFIG_INSTALL_PREFIX', os.path.abspath(conf.options.prefix))
    conf.define('CONFIG_SHARE_GCM', os.path.join(os.path.abspath(conf.options.prefix), 'share', 'gcm3d'))
    
    idx = logging_levels.index(conf.env.logging_level)
    conf.define('CONFIG_ENABLE_LOGGING', int(idx != 0))
    for (i, l) in enumerate(logging_levels[1:]):
        conf.define('CONFIG_ENABLE_LOGGING_' + l.upper(), int(i < idx))

    conf.define('CONFIG_ENABLE_ASSERTIONS', int(not conf.env.disable_assertions))

    if conf.env.logging_level != 'none':
        libs.append('liblog4cxx')

    if not conf.env.without_tests:
        libs.append('libgtest')

    if not conf.env.without_tests or not conf.env.without_launcher:
        libs.append('libboost')
    if not conf.env.without_launcher:
        libs.append('libboost_mpi')

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

    conf.write_config_header(os.path.join(conf.env.variant, 'generated_sources', 'libgcm', 'config.hpp'))
    conf.env.INCLUDES += [ 'generated_sources' ]

    conf.env.store(env_file)


def build(bld):
    '''Builds project'''

    bld.load(bld.env.LIBS, tooldir='waftools')

    libs = [l.upper() for l in bld.env.LIBS]

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

    if not bld.env.without_launcher or not bld.env.without_tests:
        bld(
            features='cxx',
            source=bld.path.ant_glob('src/launcher/util/*.cpp'),
            use=libs,
            name='launcher_util'
        )
        bld(
            features='cxx',
            source=bld.path.find_node('src/launcher/launcher.cpp'),
            use=libs,
            name='launcher'
        )
        bld(
            features='cxx',
            source=bld.path.ant_glob('src/launcher/loaders/**/*.cpp'),
            use=libs,
            name='loaders'
        )


    if not bld.env.without_launcher:
        bld(
            features='cxx cxxprogram',
            source=bld.path.find_node('src/launcher/main.cpp'),
            use=['gcm', 'launcher_util', 'loaders', 'launcher'] + libs,
            target='gcm3d'
        )
        bld.install_as('${PREFIX}/bin/gcm3d_pv_render.py', 'tools/pv_render.py')


    if not bld.env.without_tests:
        bld(
            features='cxx cxxprogram',
            source=bld.path.ant_glob('src/tests/unit/**/*.cpp'),
            use=['gcm', 'launcher_util', 'loaders'] + libs,
            target='gcm3d_unit_tests',
            install_path=None
        )
        bld(
            features='cxx cxxprogram',
            source=bld.path.ant_glob('src/tests/func/**/*.cpp'),
            use=['gcm', 'launcher_util', 'loaders', 'launcher'] + libs,
            target='gcm3d_func_tests',
            install_path=None
        )
        perf_test_sources = [x for x in bld.path.ant_glob('src/tests/perf/*.cpp') if x.name != 'util.cpp']
        bld(
            features='cxx',
            source=bld.path.find_node('src/tests/perf/util.cpp'),
            name='perf_util'
        )
        for s in perf_test_sources:
            bld(
                features='cxx cxxprogram',
                source=s,
                use=['gcm', 'perf_util', 'launcher_util'] + libs,
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
            bld.path.ant_glob('src/libgcm/**/*.hpp'),
            cwd=bld.path.find_dir('src/libgcm'),
            relative_trick=True
        )
        bld.install_files(
            '${PREFIX}/include/%s-%s/%s' % (APPNAME, VERSION, APPNAME),
            bld.path.find_node(os.path.join(out, bld.variant, 'generated_sources', 'libgcm', 'config.hpp')),
            cwd=bld.path.find_dir(os.path.join(out, bld.variant, 'generated_sources', 'libgcm')),
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
