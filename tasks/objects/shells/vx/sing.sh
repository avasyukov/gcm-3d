singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vx/papper_0_30.xml --output-dir ./result/vx/papper_z_0_30 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vx/papper_0_20.xml --output-dir ./result/vx/papper_z_0_20 &

wait 

singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vx/papper_0_10.xml --output-dir ./result/vx/papper_z_0_10
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/vx/papper_0_04.xml --output-dir ./result/vx/papper_z_0_04