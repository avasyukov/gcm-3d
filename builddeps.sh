#!/bin/bash
. setenv.sh


download_deps()
{
	cd "$DIR/deps";
	wget "http://www.cmake.org/files/v2.8/cmake-2.8.7.tar.gz"
	wget "http://www.vtk.org/files/release/5.2/vtk-5.2.1.tar.gz"
	wget "http://citylan.dl.sourceforge.net/project/tinyxml/tinyxml/2.6.2/tinyxml_2_6_2.zip"
}

build_cmake()
{
	cd "$DIR/deps";
	rm -rf cmake-2.8.7
	tar -xf cmake-2.8.7.tar.gz
	cd cmake-2.8.7
	./bootstrap \
		--prefix="$DIR/deps/"
	make
	make install
}

build_vtk()
{
	cd "$DIR/deps";
	rm -rf VTK
	tar -xf vtk-5.2.1.tar.gz
	cd VTK
	cmake \
		-DVTK_USE_RENDERING=OFF \
		-DBUILD_SHARED_LIBS=ON \
		-DCMAKE_INSTALL_PREFIX="$DIR/deps" \
		-DVTK_INSTALL_NO_DEVELOPMENT=ON \
		-DVTK_INSTALL_LIB_DIR="lib" \
		-DVTK_WRAP_PYTHON=OFF \
		-DVTK_WRAP_TCL=OFF \
		-DVTK_WRAP_JAVA=OFF 
	make clean
	make
	make install
}

build_tinyxml()
{
	cd "$DIR/deps"
	rm -rf tinyxml
	unzip tinyxml_2_6_2.zip
	cd tinyxml
	sed -i \
		-e '/^TINYXML_USE_STL/ s|=.*|=YES|' \
		-e "s|^RELEASE_CFLAGS.*|& ${CXXFLAGS} -fPIC|" Makefile
	make
	g++ -fPIC ${CXXFLAGS} -shared -o libtinyxml.so.0.2.6.2 -Wl,-soname,libtinyxml.so.0 *.o
	mkdir -p "$DIR/deps/lib"
	mkdir -p "$DIR/deps/include"
	cp libtinyxml.so.0.2.6.2 "$DIR/deps/lib/"
	cp tinyxml.h tinystr.h "$DIR/deps/include"
	cd "$DIR/deps/lib"
	ln -s libtinyxml.so.0.2.6.2 libtinyxml.so.0
	ln -s libtinyxml.so.0.2.6.2 libtinyxml.so
}

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
mkdir -p deps

download_deps
build_cmake
build_vtk
build_tinyxml
