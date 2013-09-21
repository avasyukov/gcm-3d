from waflib.Tools import waf_unit_test

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
        '--use-mpich2',
        action='store_true',
        default=False,
        help='Link against mpich2 instead of openmpi'
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
        if b:
            return 'yes'
        else:
            return 'no'

    conf.msg('Prefix', conf.options.prefix)
    conf.msg('Build static lib', yes_no(conf.options.static))
    conf.msg('Use mpich2', yes_no(conf.options.use_mpich2))
    conf.msg('Build launcher', yes_no(not conf.options.without_launcher))
    conf.msg('Enable logging', yes_no(not conf.options.without_logging))
    conf.msg('Execute tests', yes_no(not conf.options.without_tests))
    conf.msg('Install headers', yes_no(not conf.options.without_headers))
    conf.msg('Install resources', yes_no(not conf.options.without_resources))

    libs = [
        'utils',
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
    conf.env.use_mpich2 = conf.options.use_mpich2
    conf.env.static = conf.options.static

    if conf.env.use_mpich2:
        libs.append('libmpich2')
    else:
        libs.append('libopenmpi')

    conf.env.CXXFLAGS = []

    if not conf.options.without_default_cxxflags:
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

    conf.load(libs, tooldir='waftools')
    conf.env.LIBS = libs


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
            includes='src/libgcm',
            use=['gcm'] + libs,
            target='gcm3d'
        )
    
    if not bld.env.without_tests:
        bld(
            features='cxx cxxprogram test',
            source=bld.path.ant_glob('src/tests/**/*.cpp'),
            includes='src/libgcm',
            use=['gcm'] + libs,
            target='gcm3d_tests',
            install_path=None
        )
        bld.add_post_fun(waf_unit_test.summary)

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
