for (( i=0; i < 9; i++ ))
do
    cp models/aberration.geo models/aberration_000_0$i.geo
    taskset -c 0 ./gcm3d --task tasks/tests/aberration_000_0$i.xml
    rm models/aberration_000_0$i.geo
done

