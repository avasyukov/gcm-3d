BUILD       = build
DEPS_DIR    = $(BUILD)/deps

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
CCFLAGS     += -I$(shell find /usr/include -maxdepth 1 -name vtk* | head -n1)
CCFLAGS     += -Wno-deprecated
CCFLAGS     += -I./src/launcher
CCFLAGS     += -I./src/libgcm
CCFLAGS     += -I./src/
CCFLAGS     += -I/usr/include/irrlicht
CCFLAGS     += -std=c++11
CCFLAGS     += -DCONFIG_ENABLE_LOGGING
CCFLAGS     += -O3 -funroll-loops
#CCFLAGS     += -O0 -g

LIBDIRS      = $(shell uname -m | grep -q x86_64 && echo /usr/lib /usr/lib64 || echo /usr/lib)
LDFLAGS     += $(shell pkg-config --libs $(LIBS))
LDFLAGS     += -l$(shell find $(LIBDIRS) -maxdepth 1 -iname libgmsh.so -printf %f -quit | sed 's/lib\(.*\).so/\1/g' )
LDFLAGS     += -L$(shell find $(LIBDIRS) -maxdepth 1 -name vtk* -print -quit)
LDFLAGS     += -L$(BUILD)
LDFLAGS     += -Wl,-rpath=$(BUILD)
LDFLAGS     += -lrt

LIB_SOURCES       =  $(shell find src/libgcm -iname *.cpp)
LAUNCHER_SOURCES  =  $(shell find src/launcher -iname *.cpp) 

UNIT_SOURCES      =  $(shell find src/tests/unit -iname *.cpp)
UNIT_SOURCES      += $(shell find src/launcher/loaders -iname *.cpp)
UNIT_SOURCES      += src/launcher/util/xml.cpp

FUNC_SOURCES      =  $(shell find src/tests/func -iname *.cpp)
FUNC_SOURCES      += $(shell find src/launcher -iname *.cpp ! -name main.cpp)

VIEW_NORMALS_SOURCES = tools/markers-normals/main.cpp

LIB_OBJS              =   $(patsubst %.cpp, $(BUILD)/%.o, $(LIB_SOURCES))
LAUNCHER_OBJS         =   $(patsubst %.cpp, $(BUILD)/%.o, $(LAUNCHER_SOURCES))
UNIT_OBJS             =   $(patsubst %.cpp, $(BUILD)/%.o, $(UNIT_SOURCES))
FUNC_OBJS             =   $(patsubst %.cpp, $(BUILD)/%.o, $(FUNC_SOURCES))
VIEW_NORMALS_OBJS     =   $(patsubst %.cpp, $(BUILD)/%.o, $(VIEW_NORMALS_SOURCES))

DEPS =  $(LIB_SOURCES)
DEPS += $(LAUNCHER_SOURCES)
DEPS += $(FUNC_SOURCES)
DEPS += $(UNIT_SOURCES)
DEPS :=  $(DEPS:%.cpp=$(DEPS_DIR)/%.P)


default: $(GCM3D)

clean:
	@- rm -rf $(BUILD)

$(DEPS_DIR)/%.P: %.cpp
	$(shell mkdir -p $$(dirname $@))	
	$(CC) -c $(CCFLAGS) -MM -MT $(BUILD)/$*.o -MF $@ $<	


$(BUILD)/%.o: %.cpp
	$(shell mkdir -p $$(dirname $@))
	$(CC) -c $(CCFLAGS) -fpic -o $@ $<


$(GCMLIB_SO_2): $(LIB_OBJS)
	$(CC)                    \
	 -shared                 \
	 -Wl,-soname,$(GCMLIB_SO)\
	 $(LIB_OBJS)             \
	 $(LDFLAGS)              \
	 -lc                     \
	 -lgsl                   \
	 -lvtkCommon             \
	 -lvtkFiltering          \
	 -lvtkIO                 \
	 -lvtkFiltering          \
	 -lvtkCommon             \
	 -lvtkDICOMParser        \
	 -lvtkNetCDF_cxx         \
	 -lvtkNetCDF             \
	 -lvtkmetaio             \
	 -lvtksqlite             \
	 -lvtksys                \
	 -o $@

$(GCMLIB_SO_1): $(GCMLIB_SO_2)
	ln -fsr $(GCMLIB_SO_2) $(GCMLIB_SO_1)

$(GCMLIB_SO): $(GCMLIB_SO_1)
	ln -fsr $(GCMLIB_SO_1) $(GCMLIB_SO)

all: $(GCM3D) tests utils
	
tests: $(GCM3D_UNIT_TESTS) $(GCM3D_FUNC_TESTS)

utils: $(GCM3D_VIEW_NORMALS) 

$(GCM3D): $(GCMLIB_SO) $(LAUNCHER_OBJS)
	$(CC)                    \
	 $(CCFLAGS)              \
	 $(LDFLAGS)              \
	 -lgcm                   \
	 $(LAUNCHER_OBJS)        \
	 -o $@

$(GCM3D_UNIT_TESTS): $(GCMLIB_SO) $(UNIT_OBJS)
	$(CC)                    \
	 $(CCFLAGS)              \
	 $(LDFLAGS)              \
	 -lgcm                   \
	 -lgtest                 \
	 $(UNIT_OBJS)            \
	 -o $@

$(GCM3D_FUNC_TESTS): $(GCMLIB_SO) $(FUNC_OBJS)
	$(CC)                    \
	 $(CCFLAGS)              \
	 $(LDFLAGS)              \
	 -lgcm                   \
	 -lgtest                 \
	 $(FUNC_OBJS)            \
	 -o $@

$(GCM3D_VIEW_NORMALS): $(VIEW_NORMALS_OBJS) $(GCMLIB_SO)
	$(CC)                    \
	 $(CCFLAGS)              \
	 $(LDFLAGS)              \
	 -lIrrlicht              \
	 -lgcm                   \
	 $(VIEW_NORMALS_OBJS)    \
	 -o $@

doxy: $(DOXYFILE)
	$(DOXY) $^

.PHONY: default clean all doxy tests gcm3d utils
	
ifneq "$(MAKECMDGOALS)" "clean"
-include $(DEPS)
endif
