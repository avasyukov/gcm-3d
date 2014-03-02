BUILD=build

CC          =  mpic++ 
LIBS        =  libxml-2.0 liblog4cxx gsl
GCMLIB      =  libgcm
VER_MAJOR   =  0
VER_MINOR   =  1
GCMLIB_SO   =  $(BUILD)/$(GCMLIB).so
GCMLIB_SO_1 =  $(GCMLIB_SO).$(VER_MAJOR)
GCMLIB_SO_2 =  $(GCMLIB_SO_1).$(VER_MINOR)

DOXY        =  doxygen
DOXYFILE    =  tools/Doxyfile

GCM3D_UNIT_TESTS   = $(BUILD)/gcm3d_unit_tests
GCM3D_FUNC_TESTS   = $(BUILD)/gcm3d_func_tests
GCM3D_VIEW_NORMALS = $(BUILD)/gcm3d_view_normals
GCM3D              = $(BUILD)/gcm3d

CCFLAGS     += -Wall $(shell pkg-config --cflags $(LIBS))
CCFLAGS     += -I/usr/include/vtk-5.10
CCFLAGS     += -I/usr/include/irrlicht
CCFLAGS     += -Wno-deprecated
CCFLAGS     += -I./src/libgcm
CCFLAGS     += -I./src/
CCFLAGS     += -std=c++11
CCFLAGS     += -DCONFIG_ENABLE_LOGGING
CCFLAGS     += -O3 -funroll-loops
#CCFLAGS     += -O0 -g

LDFLAGS     += $(shell pkg-config --libs $(LIBS))
LDFLAGS     += -L/usr/lib64/vtk-5.10
LDFLAGS     += -L$(BUILD)
LDFLAGS     += -Wl,-rpath=$(BUILD)

LIB_SOURCES      =   $(shell find src/libgcm -iname *.cpp)
LAUNCHER_SOURCES =   $(shell find src/launcher -iname *.cpp)
UNIT_SOURCES     =   $(shell find src/tests/unit -iname *.cpp)

FUNC_SOURCES     =   $(shell find src/tests/func -iname *.cpp)
FUNC_SOURCES     +=  src/launcher/launcher.cpp
FUNC_SOURCES     +=  src/launcher/xml.cpp

LIB_OBJS         =   $(patsubst %.cpp, $(BUILD)/%.o, $(LIB_SOURCES))
LAUNCHER_OBJS    =   $(patsubst %.cpp, $(BUILD)/%.o, $(LAUNCHER_SOURCES))
UNIT_OBJS        =   $(patsubst %.cpp, $(BUILD)/%.o, $(UNIT_SOURCES))
FUNC_OBJS        =   $(patsubst %.cpp, $(BUILD)/%.o, $(FUNC_SOURCES))

default: all

clean:
	rm -rf $(BUILD)

$(BUILD)/%.o: %.cpp
	$(shell mkdir -p $$(dirname $@))
	$(CC) -c $(CCFLAGS) -fpic -o $@ $<

$(GCMLIB_SO_2): $(LIB_OBJS)
	$(CC) $(LDFLAGS)  -shared -Wl,-soname,$(GCMLIB_SO) -o $@ $(LIB_OBJS) -lc \
	 -lgsl            \
	 -lvtkCommon      \
	 -lvtkFiltering   \
	 -lvtkIO          \
	 -lvtkFiltering   \
	 -lvtkCommon      \
	 -lvtkDICOMParser \
	 -lvtkNetCDF_cxx  \
	 -lvtkNetCDF      \
	 -lvtkmetaio      \
	 -lvtksqlite      \
	 -lvtksys         \
	 -lgmsh           

$(GCMLIB_SO_1): $(GCMLIB_SO_2)
	ln -fsr $(GCMLIB_SO_2) $(GCMLIB_SO_1)

$(GCMLIB_SO): $(GCMLIB_SO_1)
	ln -fsr $(GCMLIB_SO_1) $(GCMLIB_SO)

all: $(GCM3D) $(GCM3D_UNIT_TESTS) $(GCM3D_FUNC_TESTS) $(GCM3D_VIEW_NORMALS)

$(GCM3D): $(GCMLIB_SO) $(LAUNCHER_OBJS)
	$(CC)             \
	 $(CCFLAGS)       \
	 $(LDFLAGS)       \
	 -lgcm            \
	 $(LAUNCHER_OBJS) \
	 -o $@

$(GCM3D_UNIT_TESTS): $(GCMLIB_SO) $(UNIT_OBJS)
	$(CC)             \
	 $(CCFLAGS)       \
	 $(LDFLAGS)       \
	 -lgcm            \
	 -lgtest          \
	 $(UNIT_OBJS)     \
	 -o $@

$(GCM3D_FUNC_TESTS): $(GCMLIB_SO) $(FUNC_OBJS)
	$(CC)             \
	 $(CCFLAGS)       \
	 $(LDFLAGS)       \
	 -lgcm            \
	 -lgtest          \
	 $(FUNC_OBJS)     \
	 -o $@

$(GCM3D_VIEW_NORMALS): tools/markers-normals/main.cpp
	$(CC)             \
	 $(CCFLAGS)       \
	 $(LDFLAGS)       \
	 -lIrrlicht       \
	 -lgcm            \
	 $^               \
	 -o $@

doxy: $(DOXYFILE)
	$(DOXY) $^

.PHONY: default clean all doxy
