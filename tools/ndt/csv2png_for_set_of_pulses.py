#usage (the first number is for vertical dilatation, the second is for cutting the initial signal):
#python3 tools/ndt/csv2png_for_set_of_pulses.py aberration_000 5 100

import matplotlib.pyplot as plt
import sys
import png

values = []
for task in range(0, 40):
        values_task = []
        for n in range(20):
                fname = sys.argv[1] + "_" + format(task, '02d') + "_" + str(n) + ".csv"
                with open(fname) as f:
                        lines = f.readlines()
                        vz = [(float)(line.split()[3]) for line in lines]
                        values_task.append(vz)
        for times in range(int(sys.argv[2])):
                values.append([sum(x) for x in zip(*values_task)][int(sys.argv[3]):])

min_len = min(map(len, values))
min_v = min(map(min, values))
values_upg = [[x - min_v for x in l[0:min_len]] for l in values]

max_v = max(map(max, values_upg))
v_color = [[int(255*x/max_v) for x in l] for l in values_upg]

with open(sys.argv[1] + ".png", 'wb') as f:
    w = png.Writer(min_len, len(v_color), greyscale=True)
    w.write(f, v_color)

