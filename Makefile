production: main.cpp
	g++ -O3 -o gcm3d -I/usr/include/vtk/ -Wno-deprecated -lvtkCommon -lvtkVolumeRendering -lvtkFiltering -lvtkIO -lgsl -lgslcblas -lm main.cpp

test: main.cpp
	g++ -o gcm3d_test -I/usr/include/vtk/ -Wno-deprecated -lvtkCommon -lvtkVolumeRendering -lvtkFiltering -lvtkIO -lgsl -lgslcblas -lm main.cpp

profile: main.cpp
	g++ -o gcm3d_gprof -I/usr/include/vtk/ -Wno-deprecated -lvtkCommon -lvtkVolumeRendering -lvtkFiltering -lvtkIO -lgsl -lgslcblas -lm -pg main.cpp

tools: plot2d create_cube

plot2d:
	g++ -o tools/plot2d tools/plot2d.cpp

create_cube:
	g++ -o tools/create_cube tools/create_cube.c

clean: clean_binary clean_test_data

clean_binary:
	rm -f ./gcm3d ./gcm3d_test ./gcm3d_gprof ./tools/plot2d ./tools/create_cube

clean_test_data:
	rm -f ./*.vtk ./*.vtu ./log
