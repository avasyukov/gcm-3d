#!/bin/sh
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
mpirun -np $( cat /proc/cpuinfo | grep processor | wc -l ) pvbatch --use-offscreen-rendering "$DIR/render_snapshots.py"
