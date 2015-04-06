#!/usr/bin/env python
import os
import imp

def get_values(env, prefix):
    values = []
    for v in dir(env):
        if v.startswith(prefix):
            values.extend(getattr(env, v))
    return set(values)

def FlagsForFile(filename, **kwargs):

    flags = {
        'flags': [
            '-Wall',
            '-Wextra',
            '-Werror',
            '-fexceptions',
            '-DNDEBUG',
            '-std=c++11'
        ],
        'do_cache': True
    }

    env_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'build', 'current_env.py')
    
    if os.path.exists(env_file):
        env = imp.load_source('env', env_file)
    else:
        return flags

    variant_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'build', env.variant)

    for d in env.INCLUDES:
        if os.path.isdir(d):
            flags['flags'].append('-I' + d)
        else:
            d = os.path.join(variant_dir, d)
            if os.path.isdir(d):
                flags['flags'].append('-I' + d)

    for include in get_values(env, 'INCLUDES_LIB'):
        flags['flags'].append('-I' + include)
    
    for libpath in get_values(env, 'LIBPATH_LIB'):
        flags['flags'].append('-L' + libpath)
    
    for lib in get_values(env, 'LIB_LIB'):
        flags['flags'].append('-l' + lib)

    return flags

if __name__ == '__main__':
    print(FlagsForFile(1))
