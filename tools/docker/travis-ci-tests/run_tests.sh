#!/bin/sh

cd /root/gcm-3d
./build/gcm3d_unit_tests
r1=$?
./build/gcm3d_func_tests
r2=$? 

exit $(( $r1 || $r2 ))
