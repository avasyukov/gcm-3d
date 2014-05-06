#!/bin/bash

OPTIND=1
cpus=`cat /proc/cpuinfo | grep processor | tail -n 1 | sed -e 's/.*: //'`
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

rm -R *vtu *vts
echo -e "Complete command:\n\ttime mpirun -np 1 ./build/optimize/gcm3d --data-dir . --task $task"
time mpirun -np 1 ./build/optimize/gcm3d --data-dir . --task $task
