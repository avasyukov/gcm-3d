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
        values.append([[float(x) for x in l.split()] for l in f.readlines()][0:args.max_length])

for i in range(len(values)):
    for j in range(len(values[0])):
        for k in range(len(values[0][0])):
            values[0][j][k] += values[i][j][k]

with open(args.output, 'w') as f:
    for j in range(len(values[0])):
        print(values[0][j][0], values[0][j][1], values[0][j][2], values[0][j][3], values[0][j][4], values[0][j][5], values[0][j][6], values[0][j][7], values[0][j][8], file=f)
    f.close
