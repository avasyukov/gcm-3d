#!/usr/bin/python3

import scipy.misc
import numpy as np
import sys
import math


def interpolate(v1, v2, factor):
    return v1*(1-factor) + v2*factor

ANGLE = 60

if len(sys.argv) != 3:
    print('Usage: sectorize.py <in> <out>', file=sys.stderr)
    sys.exit(-1)

data = scipy.misc.imread(sys.argv[1])

h = len(data)
w = len(data[0])

angle_rad = ANGLE*math.pi/180

r = h/angle_rad
R = r + w
q = r*math.cos(angle_rad/2)

nh = math.ceil(R - q)
nw = math.ceil(2*R*math.sin(angle_rad/2))

new_data = np.zeros((nh, nw, 3))

cx = nw/2
cy = q

for i in range(nh):
    for j in range(nw):
        x = j + 0.5
        y = i + 0.5
        dx = x - cx
        dy = cy + y

        d = (dx**2 + dy**2)**0.5 - r

        d1 = math.floor(d)
        d2 = math.ceil(d)

        a = (0.5 + math.atan2(dx, dy)/angle_rad)*h

        ray1 = math.floor(a)
        ray2 = math.ceil(a)

        if all([0 <= d1 < w, 0 <= d2 < w, 0 <= ray1 < h, 0 <= ray2 < h]):
            r1v = interpolate(data[ray1][d1], data[ray1][d2], math.modf(d)[0])
            r2v = interpolate(data[ray2][d1], data[ray2][d2], math.modf(d)[0])
            v = interpolate(r1v, r2v, math.modf(a)[0])
            new_data[i][j] = v
        else:
            new_data[i][j] = (0, 0, 0)

scipy.misc.imsave(sys.argv[2], new_data)
