#usage:
#python3 tools/ndt/csv2png_for_single_pulse.py aberrator_000_00

import matplotlib.pyplot as plt
import sys
import png

values = []
for n in range(20):
        fname = sys.argv[1] + "_" + str(n) + ".csv"
        with open(fname) as f:
                lines = f.readlines()
                t  = [(float)(line.split()[0]) for line in lines]
                vz = [(float)(line.split()[3]) for line in lines]
                values.append(vz)

                #plt.plot(t, vz)
                #plt.savefig(sys.argv[1] + "_" + str(n) + ".png")
                #plt.clf()

min_len = min(map(len, values))
min_v = min(map(min, values))
values_upg = [[x - min_v for x in l[0:min_len]] for l in values]

max_v = max(map(max, values_upg))
v_color = [[int(255*x/max_v) for x in l] for l in values_upg]

with open(sys.argv[1] + ".png", 'wb') as f:
    w = png.Writer(min_len, len(v_color), greyscale=True)
    w.write(f, v_color)

