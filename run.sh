#!/bin/bash

CPUS=1
if [ $# -ne 0 ]
then
	CPUS=$1
fi

time LD_LIBRARY_PATH=/usr/lib64/openmpi/lib/:. mpirun -np $CPUS ./opengcm
