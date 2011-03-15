test: main.cpp
	g++ -O3 -o gcm_test -lgsl -lgslcblas -lm main.cpp

profile: main.cpp
	g++ -o gcm_gprof_test -lgsl -lgslcblas -lm -pg main.cpp

tools: plot2d create_cube

plot2d:
	g++ -o tools/plot2d tools/plot2d.cpp

create_cube:
	g++ -o tools/create_cube tools/create_cube.c

clean:
	rm -f ./gcm_test ./gcm_prod_test ./gcm_gprof_test ./tools/plot2d ./tools/create_cube

clean_data:
	rm -f ./*.vtk ./log
