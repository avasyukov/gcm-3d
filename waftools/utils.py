import sys
from waflib.Configure import conf


@conf
def check_linux(conf):
    if not sys.platform.startswith('linux'):
        conf.fatal('Only Linux platform is supported at the moment')

