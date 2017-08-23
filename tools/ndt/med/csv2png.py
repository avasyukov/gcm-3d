#!/usr/bin/python3

import argparse
import os
import re
import png


parser = argparse.ArgumentParser(description='Converts sensor data from CSV to PNG')

parser.add_argument(
    'file',
    metavar='FILE',
    help='CSV file with sensor data'
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

#regexp = re.compile(args.number_regexp)
#files = []
#for f in args.files:
#fname = os.path.basename(file)
#m = regexp.match(fname)
#if m:
#    file_in = int(m.group(1), file)

#files.sort(key=lambda x: x[0])

#values = []
#for (_, fpath) in files:
with open(args.file) as f:
    values = [[int(x) for x in l.split()] for l in f.readlines()]#[0:args.max_length]

#sxx = [[abs(x[1]) for x in v] for v in values]
#max_sxx = max(map(max, sxx))

#sxx_color = [[int(255*x/max_sxx) for x in l] for l in sxx]


with open(args.output, 'wb') as f:
    w = png.Writer(len(values[0]), len(values), greyscale=True)
    w.write(f, values)

