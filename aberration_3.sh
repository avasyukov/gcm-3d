for (( i=0; i < 9; i++ ))
do
    cp models/aberration.geo models/aberration_000_3$i.geo
    taskset -c 3 ./gcm3d --task tasks/tests/aberration_000_3$i.xml
    rm models/aberration_000_3$i.geo
done

