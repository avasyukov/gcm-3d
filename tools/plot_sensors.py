#!/bin/env python3

import sys
import subprocess

values = ['vx', 'vy', 'vz', 'sxx', 'sxy', 'sxz', 'syy', 'syz', 'szz']

for (i,v) in enumerate(values):
    p = subprocess.Popen(['gnuplot'], stdin=subprocess.PIPE)
    script  = "set terminal pngcairo size 500,500 enhanced font 'Verdana,10';\n"
    script += "set output '%s.png';\n" % v
    script += "set yrange [-5:+5];\n"
    script += "plot "
    for f in sys.argv[1:]:
        script += "'%s' using :%d with linespoints," % (f, 2+i)
    script = script[:-1] + ';\n'
    p.communicate(input=str.encode(script))
