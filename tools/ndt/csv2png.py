#!/usr/bin/python3

import argparse
import os
import re
import png


parser = argparse.ArgumentParser(description='Converts sensor data from CSV to PNG')

parser.add_argument(
    'files',
    metavar='FILES',
    nargs='+',
    help='List of CSV files with sensor data'
)

parser.add_argument(
    '-r',
    '--number-regexp',
    required=True,
    metavar='REGEXP',
    help='Regexp to extract frame number from file name'
)

parser.add_argument(
    '-o',
    '--output',
    required=True,
    metavar='FILE',
    help='Output file name'
)

parser.add_argument(
    '-m',
    '--max-length',
    default=9999,
    type=int,
    metavar='LEN',
    help='Max length of sensor data'
)

parser.add_argument(
    '-s',
    '--skip',
    default=80,
    type=int,
    metavar='SKIP',
    help='Number of points to skip'
)


args = parser.parse_args()

regexp = re.compile(args.number_regexp)
files = []
for f in args.files:
    fname = os.path.basename(f)
    m = regexp.match(fname)
    if m:
        files.append((int(m.group(1)), f))

files.sort(key=lambda x: x[0])

values = []
for (_, fpath) in files:
    with open(fpath) as f:
        values.append([[float(x) for x in l.split()] for l in f.readlines()[args.skip:args.max_length]])

sxx = [[abs(x[1]) for x in v] for v in values]
max_sxx = max(map(max, sxx))

sxx_color = [[int(255*x/max_sxx) for x in l] for l in sxx]


with open(args.output, 'wb') as f:
    w = png.Writer(len(sxx_color[0]), len(sxx_color), greyscale=True)
    w.write(f, sxx_color)

