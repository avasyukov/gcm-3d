for (( i=0; i < 9; i++ ))
do
    cp models/aberration.geo models/aberration_000_1$i.geo
    taskset -c 1 ./gcm3d --task tasks/tests/aberration_000_1$i.xml
    rm models/aberration_000_1$i.geo
done

