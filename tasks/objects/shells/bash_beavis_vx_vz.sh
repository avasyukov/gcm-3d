singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vx/papper_0_50.xml --output-dir ./result/vx/papper_0_50 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vx/papper_0_40.xml --output-dir ./result/vx/papper_0_40 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vx/papper_0_30.xml --output-dir ./result/vx/papper_0_30 &

singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vz/papper_0_50.xml --output-dir ./result/vz/papper_0_50 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vz/papper_0_40.xml --output-dir ./result/vz/papper_0_40 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vz/papper_0_30.xml --output-dir ./result/vz/papper_0_30 &

wait

singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vx/papper_0_20.xml --output-dir ./result/vx/papper_0_20 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vx/papper_0_10.xml --output-dir ./result/vx/papper_0_10 &

singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vz/papper_0_90.xml --output-dir ./result/vz/papper_0_90 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vz/papper_0_80.xml --output-dir ./result/vz/papper_0_80 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vz/papper_0_70.xml --output-dir ./result/vz/papper_0_70 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vz/papper_0_60.xml --output-dir ./result/vz/papper_0_60 &

wait 

singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vz/papper_0_20.xml --output-dir ./result/vz/papper_0_20 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vz/papper_0_10.xml --output-dir ./result/vz/papper_0_10 &

singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vx/papper_0_90.xml --output-dir ./result/vx/papper_0_90 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vx/papper_0_80.xml --output-dir ./result/vx/papper_0_80 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vx/papper_0_70.xml --output-dir ./result/vx/papper_0_70 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vx/papper_0_60.xml --output-dir ./result/vx/papper_0_60 &

wait

singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vx/papper_0_04.xml --output-dir ./result/vx/papper_0_04 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vx/papper_1_00.xml --output-dir ./result/vx/papper_1_00 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vz/papper_1_00.xml --output-dir ./result/vz/papper_1_00 &

wait

singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vz/papper_0_04.xml --output-dir ./result/vz/papper_0_04 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vx/papper_1_00.xml --output-dir ./result/vx/papper_1_00 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vz/papper_1_00.xml --output-dir ./result/vz/papper_1_00 &
