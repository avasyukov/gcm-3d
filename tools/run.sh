#!/bin/bash

OPTIND=1
cpus="1"
task=""

while getopts "n:t:" opt; do
    case "$opt" in
    n)  cpus=$OPTARG
        ;;
    t)  task=$OPTARG
        ;;
    esac
done

shift $((OPTIND-1))

if [[ -z $task ]]; then
    task=$@;
fi

echo -e "Complete command:\n\ttime LD_LIBRARY_PATH=/usr/lib64/openmpi/lib/:./build/ mpirun -np $cpus ./build/gcm3d --data-dir . --task $task"
time LD_LIBRARY_PATH=/usr/lib64/openmpi/lib/:./build/ mpirun -np $cpus ./build/gcm3d --data-dir . --task $task
