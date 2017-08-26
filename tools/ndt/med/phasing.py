#!/usr/bin/python3

import argparse
import os
import re
import png
import math
import csv
import numpy as np
from scipy.signal import hilbert
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser(description='Converts sensor data from raw to B-scan')

parser.add_argument(
    'files',
    metavar='FILES',
    nargs='+',
    help='List of CSV files with solo sensor data'
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
    default=1000,
    type=int,
    metavar='MAX_LENGTH',
    help='Max length of sensor data'
)

parser.add_argument(
    '--rays-num',
    default=200,
    type=int,
    metavar='RAYS_NUM',
    help='Number of rays'
)

parser.add_argument(
    '-f',
    '--focus',
    required=True,
    type=int,
    metavar='FOCUS',
    help='Focus distnce'
)

parser.add_argument(
    '--freq_min',
    required=True,
    type=int,
    metavar='FREQ_MIN',
    help='Min range of the frequency window'
)

parser.add_argument(
    '--freq_max',
    required=True,
    type=int,
    metavar='FREQ_MAX',
    help='Max range of the frequency window'

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

#readin' data from files as a 3D array
values = []
for (_, fpath) in files:
    with open(fpath) as f:
        values.append([[int(x) for x in l.split()] for l in f.readlines()])

#dekulbergin' data: from 153 pixels of height to 48 as is was supposed to be
values_compressed = [[[0 for j in range(len(values[0][0]))] for i in range(len(values))] for k in range(len(values))]
ratio = len(values[0])/len(values)
for k, snap in enumerate(values):
    for i, row in enumerate(snap):
        for j,pixel in enumerate(row):
            values_compressed[k][int(i/ratio)][j] = pixel

#calculatin' delays - they only depend on sensor index
delay = [0 for i in range(len(values))]
res = [[0 for j in range(args.max_length)] for i in range(args.rays_num)]
for rays in range(args.rays_num):
    alpha = (math.pi*2.0/3.0/args.rays_num)*(args.rays_num/2-rays)
    s = math.sin(alpha)
    c = math.cos(alpha)
    for k, snap in enumerate(values_compressed):
        h = len(values_compressed)/2 - k
        r = math.sqrt(math.pow(args.focus*c, 2) + math.pow(args.focus*s - h, 2))
        delay[k] = args.focus - r

    #calculatin' each ray of result with necessary delays
    for k, snap in enumerate(values_compressed):
        for i,row in enumerate(snap):
            for j in range(len(res[i])):
                lower = j - int(math.ceil(delay[k] + delay[i]))
                higher = j - int(math.floor(delay[k] + delay[i]))
                if (lower != higher):
                    delta = abs(delay[k] + delay[i] - math.floor(delay[k] + delay[i]))
                    res[rays][j] += row[lower] + delta*(row[higher] - row[lower])
                else:
                    res[rays][j] += row[lower]

#Narrowband filtering + Hilbert transformation
res_fft = []
for i,row in enumerate(res):
    fft = np.fft.rfft(row)

    #this is to check the spectrum and find the carrying freauency
    #plt.plot(np.abs(fft)[10:]) #near-zero freqencies somehow are exremely large
    #plt.savefig(args.output + "_fft_" + str(i) + ".png")
    #plt.clf()

    for j,freq in enumerate(fft):
        if not (args.freq_min <= j <= args.freq_max):
            fft[j] = 0
    res_fft.append(np.abs(hilbert(np.fft.irfft(fft))))

#results -> 255 grayscale
max_res_fft = max(map(max, res_fft))
res_fft_color = [[int(255*math.pow(x/max_res_fft, 0.25)) for x in l] for l in res_fft]
max_res = max(map(max, res))
min_res = min(map(min, res))
res_color = [[int(255*(x-min_res)/(max_res-min_res)) for x in l] for l in res]

#printin'
with open(args.output+".png", 'wb') as f:
    w = png.Writer(len(res_color[0]), len(res_color), greyscale=True)
    w.write(f, res_color)
with open(args.output+"_fft.png", 'wb') as f:
    w = png.Writer(len(res_fft_color[0]), len(res_fft_color), greyscale=True)
    w.write(f, res_fft_color)

