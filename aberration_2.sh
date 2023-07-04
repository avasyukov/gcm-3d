for (( i=0; i < 9; i++ ))
do
    cp models/aberration.geo models/aberration_000_2$i.geo
    taskset -c 2 ./gcm3d --task tasks/tests/aberration_000_2$i.xml
    rm models/aberration_000_2$i.geo
done

