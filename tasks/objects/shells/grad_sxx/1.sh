./build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_1_00.xml --output-dir ./result/grad_sxx/papper_z_1_00 &
./build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_0_90.xml --output-dir ./result/grad_sxx/papper_z_0_90 &
./build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_0_80.xml --output-dir ./result/grad_sxx/papper_z_0_80 &
./build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_0_70.xml --output-dir ./result/grad_sxx/papper_z_0_70 &
./build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_0_60.xml --output-dir ./result/grad_sxx/papper_z_0_60 &


wait

./build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_0_50.xml --output-dir ./result/grad_sxx/papper_z_0_50 &
./build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_0_40.xml --output-dir ./result/grad_sxx/papper_z_0_40 &
./build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_0_30.xml --output-dir ./result/grad_sxx/papper_z_0_30 &

wait 

./build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_0_20.xml --output-dir ./result/grad_sxx/papper_z_0_20
./build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_0_10.xml --output-dir ./result/grad_sxx/papper_z_0_10
./build/gcm3d --task ./tasks/objects/shells/grad_sxx/papper_0_04.xml --output-dir ./result/grad_sxx/papper_z_0_04