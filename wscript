VERSION = '0.1'
APPNAME = 'gcm3d'


def options(opt):
    '''Sets gcm3d specific options'''

    opt.add_option(
        '--with-launcher',
        action='store_true',
        default=False,
        help='Build gcm3d launcher'
    )

    opt.add_option(
        '--without-logging',
        action='store_true',
        default=False,
        help='Disable libgcm logging routines'
    )

    opt.add_option(
        '--without-default-cxxflags',
        action='store_true',
        default=False,
        help='Disable default cxxflags'
    )

    opt.load('compiler_cxx')
    opt.load('utils', tooldir='waftools')


def configure(conf):
    '''Configures build environment'''

    def yes_no(b):
        return 'yes' if b else 'no'

    conf.msg('Prefix', conf.options.prefix)
    conf.msg('Build launcher', yes_no(conf.options.with_launcher))
    conf.msg('Enable logging', yes_no(not conf.options.without_logging))

    libs = [
        'utils',
        'libgsl',
        'libxml2',
        'libgmsh',
        'libopenmpi',
        'libvtk'
    ]

    conf.env.with_launcher = conf.options.with_launcher
    conf.env.without_logging = conf.options.without_logging

    if not conf.env.without_logging:
        libs.append('liblog4cxx')

    if conf.env.with_launcher:
        libs.append('libxmlpp')

    conf.load(libs, tooldir='waftools')
    conf.env.LIBS = libs

    conf.env.CXXFLAGS = []

    if not conf.options.without_default_cxxflags:
        conf.env.CXXFLAGS += ['-Wno-deprecated']


def build(bld):
    '''Builds project'''

    bld.load(bld.env.LIBS, tooldir='waftools')

    libs = [l.upper() for l in bld.env.LIBS]

    bld(
        features='cxx cxxshlib',
        source=bld.path.ant_glob('src/libgcm/**/*.cpp'),
        use=libs,
        target='gcm'
    )

    if bld.env.with_launcher:
        bld.env.INCLUDES_LIBGCM = ['src/libgcm']
        bld.env.LIBPATH_LIBGCM = [bld.bldnode.abspath()]
        bld.env.LIB_LIBGCM = ['gcm']

        bld(
            features='cxx cxxprogram',
            source=['src/launcher/launcher.cpp'],
            use=['LIBGCM'] + libs,
            target='launcher'
        )
