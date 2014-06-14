#!/usr/bin/python

import os
import time



i = 2
while True:
	if os.path.exists('snap_mesh_main_cpu_0_step_' + str(i) + '.vtu'):
		os.system('mv snap_mesh_main_cpu_0_step_' + str(i-2) + '.vtu ~/dos/gcm-results')
		i = i + 1
	else:
		if os.path.exists('dump_mesh_main_cpu_0_step_' + str(i-1) + '.vtu'):
			os.system('mv snap_mesh_main_cpu_0_step_' + str(i-2) + '.vtu ~/dos/gcm-results')
			os.system('mv snap_mesh_main_cpu_0_step_' + str(i-1) + '.vtu ~/dos/gcm-results')
			os.system('rm dump_mesh_main_cpu_0_step_' + str(i-1) + '.vtu')
			quit()
		else:
			print "Waiting...\n"
			time.sleep(30)
