import sys
import math

with open("models/aberrator/sine_wide_low.txt", 'w') as f:
    for i in range(0, 145):
        f.write(str(700 + 100*math.sin(3 * math.pi * float(i)/145 + math.pi)))
        f.write('\n')
#with open("models/aberrator/angle_1100.txt", 'w') as f:
#    amp = 500
#    for i in range(0, 24):
#        f.write(str(600))
#        f.write('\n')
#
#    for i in range(24, 72):
#        f.write(str(600 + amp*(i - 24)/48))
#        f.write('\n')
#
#    for i in range(72, 120):
#        f.write(str(600 + amp*(120 - i)/48))
#        f.write('\n')
#
#    for i in range(120, 145):
#        f.write(str(600))
#        f.write('\n')

