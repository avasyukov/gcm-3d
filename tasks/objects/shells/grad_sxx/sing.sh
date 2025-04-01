singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_1_00.xml --output-dir ./result/grad_sxx/papper_z_1_00 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_0_90.xml --output-dir ./result/grad_sxx/papper_z_0_90 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_0_80.xml --output-dir ./result/grad_sxx/papper_z_0_80 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_0_70.xml --output-dir ./result/grad_sxx/papper_z_0_70 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_0_60.xml --output-dir ./result/grad_sxx/papper_z_0_60 &

wait

singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_0_50.xml --output-dir ./result/grad_sxx/papper_z_0_50 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_0_40.xml --output-dir ./result/grad_sxx/papper_z_0_40 &
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_0_30.xml --output-dir ./result/grad_sxx/papper_z_0_30 &

wait 

singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_0_20.xml --output-dir ./result/grad_sxx/papper_z_0_20
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_0_10.xml --output-dir ./result/grad_sxx/papper_z_0_10
singularity exec gcm3d-test.sif /opt/gcm3d/build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_0_04.xml --output-dir ./result/grad_sxx/papper_z_0_04