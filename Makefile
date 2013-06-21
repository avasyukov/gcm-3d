#CC          =  $(shell whereis -b mpic++ | cut -f2 -d:)
CC          = mpic++ 
#MAKEFLAGS   += --quiet
LIBS        =  libxml++-2.6 libxml-2.0 liblog4cxx
GCMLIB      =  libgcm
VER_MAJOR   =  0
VER_MINOR   =  1
GCMLIB_SO   =  $(GCMLIB).so
GCMLIB_SO_1 =  $(GCMLIB_SO).$(VER_MAJOR)
GCMLIB_SO_2 =  $(GCMLIB_SO_1).$(VER_MINOR)
CCFLAGS     += -Wall -O3 $(shell pkg-config --cflags $(LIBS))
LDFLAGS     += $(shell pkg-config --libs $(LIBS))
SKIPSRC     =  src/opengcm.cpp
SOURCES     =  $(filter-out $(SKIPSRC), $(shell find src/ -iname *.cpp))
OBJS        =  $(patsubst %.cpp, %.o, $(SOURCES))

default: all

clean:
	rm -f $(OBJS) $(GCMLIB_SO) $(GCMLIB_SO_1) $(GCMLIB_SO_2) opengcm

%.o: %.cpp
	$(CC) -I/usr/include/vtk/ -Wno-deprecated -c $(CCFLAGS) -fpic -o $*.o $*.cpp

lib: $(OBJS)
	$(CC) $(LDFLAGS)  -shared -Wl,-soname,$(GCMLIB_SO) -o $(GCMLIB_SO_2) $(OBJS) -lc
	[ -e $(GCMLIB_SO_1) ] || ln -s $(GCMLIB_SO_2) $(GCMLIB_SO_1)
	[ -e $(GCMLIB_SO) ] || ln -s $(GCMLIB_SO_1) $(GCMLIB_SO)

all: opengcm

opengcm: lib
	$(CC) -I/usr/include/vtk/ -Wno-deprecated  $(CCFLAGS) $(LDFLAGS) -L. -lgcm -lgsl -lvtkCommon -lvtkFiltering -lvtkIO -lvtkFiltering -lvtkCommon -lvtkDICOMParser -lvtkNetCDF_cxx -lvtkNetCDF -lvtkmetaio -lvtksqlite -lvtksys -lgmsh src/opengcm.cpp -o opengcm
