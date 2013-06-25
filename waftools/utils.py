import sys
from waflib.Configure import conf


@conf
def check_linux(conf):
    if not sys.platform.startswith('linux'):
        conf.fatal('Only Linux platform is supported at the moment')


def options(opt):
    opt.add_option(
        '--includepath',
        action='append',
        default=[],
        help='Additional include paths'
    )

    opt.add_option(
        '--cxxflags',
        action='append',
        default=[],
        help='Additional arguments for compiler'
    )

    opt.add_option(
        '--libpath',
        action='append',
        default=[],
        help='Additional paths to search for libraries'
    )


def configure(conf):
    conf.msg('Additional compiler flags', ' '.join(conf.options.cxxflags))
    conf.msg('Additional include paths', ','.join(conf.options.includepath))
    conf.msg('Additional paths to search libraries', ','.join(conf.options.libpath))

    conf.env.LIBPATH_UTILS = conf.options.libpath
    conf.env.INCLUDES_UTILS = conf.options.includepath
    conf.env.CXXFLAGS_UTILS = conf.options.cxxflags
