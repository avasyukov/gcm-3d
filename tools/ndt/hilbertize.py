#!/usr/bin/python3
import sys
import numpy as np
from scipy.signal import hilbert

skip, _min, _max = map(int, sys.argv[2:])
original = [[float(x) for x in l.split()] for l in  open(sys.argv[1]).readlines()[skip:]]
values = []
for i in range(len(original[0])):
    if i == 0:
        values.append([l[0] for l in original])
    else:
        fft = np.fft.rfft([l[i] for l in original])
        for j in range(len(fft)):
            if not (_min <= j <= _max):
                fft[j] = 0
        values.append(np.abs(hilbert(np.fft.irfft(fft))))
        # values.append(np.fft.irfft(fft))

with open('{}.hilbertized'.format(sys.argv[1]), 'w') as f:
    for l in zip(*values):
        print(*l, file=f)
