cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules")

project (gcm3d)

# set(WITHOUT_LOG4CXX 1)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE DEBUG CACHE STRING "Choose the type of build, options are: DEBUG, RELEASE" FORCE)
endif()

if(WITHOUT_LOG4CXX)
    set(CMAKE_CONFIG_ENABLE_LOGGING 0)
else()
    set(CMAKE_CONFIG_ENABLE_LOGGING 1)
endif()
set(CMAKE_CONFIG_ENABLE_PLUGINS 1)

if(CMAKE_BUILD_TYPE EQUAL "DEBUG")
    set(CMAKE_CONFIG_ENABLE_LOGGING_FATAL 1)
    set(CMAKE_CONFIG_ENABLE_LOGGING_ERROR 1)
    set(CMAKE_CONFIG_ENABLE_LOGGING_WARN  1)
    set(CMAKE_CONFIG_ENABLE_LOGGING_INFO  1)
    set(CMAKE_CONFIG_ENABLE_LOGGING_DEBUG 1)
    set(CMAKE_CONFIG_ENABLE_LOGGING_TRACE 0)
    set(CMAKE_CONFIG_ENABLE_ASSERTIONS    1)
else()
    set(CMAKE_CONFIG_ENABLE_LOGGING_FATAL 1)
    set(CMAKE_CONFIG_ENABLE_LOGGING_ERROR 1)
    set(CMAKE_CONFIG_ENABLE_LOGGING_WARN  1)
    set(CMAKE_CONFIG_ENABLE_LOGGING_INFO  1)
    set(CMAKE_CONFIG_ENABLE_LOGGING_DEBUG 0)
    set(CMAKE_CONFIG_ENABLE_LOGGING_TRACE 0)
    set(CMAKE_CONFIG_ENABLE_ASSERTIONS    0)
endif()


# set(CMAKE_CXX_FLAGS "-std=c++17 -fno-omit-frame-pointer -frecord-gcc-switches -fopenmp")
set(CMAKE_CXX_FLAGS "-std=c++11 -fno-omit-frame-pointer -frecord-gcc-switches -fopenmp")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -g")

find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

find_package(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})
# include_directories("/usr/include/vtk-9.1")

#find_package(MPI REQUIRED)
#include_directories(${MPI_CXX_INCLUDE_PATH})

if(NOT WITHOUT_LOG4CXX)
    find_package(Log4cxx REQUIRED)
    include_directories(${LOG4CXX_INCLUDE_DIRS})
endif()

find_package(GSL REQUIRED)
include_directories(${GSL_INCLUDE_DIRS})

find_package(Boost REQUIRED COMPONENTS filesystem system program_options serialization)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Gmsh REQUIRED)
include_directories(${GMSH_INCLUDE_DIR})
#link_directories(${GMSH_LIB_DIR})

if(WITH_TESTS)
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})
endif()

if(VTK_MAJOR_VERSION EQUAL 5)
    set(CONFIG_VTK_5 1)
elseif (VTK_MAJOR_VERSION GREATER 5)
    set(CONFIG_VTK_6 1)
else()
    message(FATAL_ERROR "Unsupported VTK version found: ${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}")
endif()


file(GLOB_RECURSE LIB_SOURCES        "${CMAKE_CURRENT_LIST_DIR}/src/libgcm/*.cpp")
if(WITH_TESTS)
    file(GLOB_RECURSE UNIT_TESTS_SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/tests/unit/*.cpp")
    file(GLOB_RECURSE FUNC_TESTS_SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/tests/func/*.cpp")
    file(GLOB_RECURSE PERF_TESTS_SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/tests/perf/*.cpp")
endif()

file(GLOB_RECURSE LAUNCHER_UTILS_SOURCES   "${CMAKE_CURRENT_LIST_DIR}/src/launcher/util/*.cpp")
file(GLOB_RECURSE LAUNCHER_LOADERS_SOURCES   "${CMAKE_CURRENT_LIST_DIR}/src/launcher/loaders/*.cpp")

file(GLOB AVAILABLE_PLUGINS   "${CMAKE_CURRENT_LIST_DIR}/src/plugins/*")

set(LAUNCHER_COMMON_SOURCES
    ${LAUNCHER_UTILS_SOURCES}
    ${LAUNCHER_LOADERS_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/src/launcher/launcher.cpp
)

set(CMAKE_CONFIG_SHARE_GCM "${CMAKE_INSTALL_PREFIX}/share/gcm3d")

configure_file("${CMAKE_CURRENT_LIST_DIR}/cmake/config.hpp.in" "${CMAKE_CURRENT_BINARY_DIR}/generated_sources/libgcm/config.hpp")
include_directories("${CMAKE_CURRENT_LIST_DIR}/src/" "${CMAKE_CURRENT_BINARY_DIR}/generated_sources/")


add_library(gcm SHARED ${LIB_SOURCES})
add_library(launcher OBJECT ${LAUNCHER_COMMON_SOURCES})
add_executable(gcm3d "${CMAKE_CURRENT_LIST_DIR}/src/launcher/main.cpp" $<TARGET_OBJECTS:launcher>)

if(WITH_TESTS)
    add_executable(gcm3d_unit_tests ${UNIT_TESTS_SOURCES} $<TARGET_OBJECTS:launcher>)
    add_executable(gcm3d_func_tests ${FUNC_TESTS_SOURCES} $<TARGET_OBJECTS:launcher>)

    add_library(perf_test_util OBJECT "${CMAKE_CURRENT_LIST_DIR}/src/tests/perf/util.cpp")
    foreach(perftestsource ${PERF_TESTS_SOURCES})
        get_filename_component(perftest "${perftestsource}" NAME_WE)
        if(NOT ("${perftest}" STREQUAL "util"))
            add_executable("gcm3d_perf_test_${perftest}" "${perftestsource}" $<TARGET_OBJECTS:perf_test_util> $<TARGET_OBJECTS:launcher>)
            target_link_libraries("gcm3d_perf_test_${perftest}" gcm)
        endif()
    endforeach()
endif()

foreach(plugin ${AVAILABLE_PLUGINS})
    file(GLOB_RECURSE PLUGIN_SOURCES   "${plugin}/*.cpp")
    get_filename_component(plugin_name "${plugin}" NAME_WE)
    add_library("gcm_${plugin_name}" SHARED ${PLUGIN_SOURCES})
    target_link_libraries("gcm_${plugin_name}" gcm)
endforeach()

#target_link_libraries(gcm ${MPI_CXX_LIBRARIES})
target_link_libraries(gcm ${LIBXML2_LIBRARIES})
if(NOT WITHOUT_LOG4CXX)
    target_link_libraries(gcm ${LOG4CXX_LIBRARIES})
endif()
target_link_libraries(gcm ${GSL_LIBRARIES})
target_link_libraries(gcm ${Boost_LIBRARIES})
target_link_libraries(gcm ${GMSH_LIBRARY})
target_link_libraries(gcm ${VTK_LIBRARIES})

target_link_libraries(gcm3d gcm)

if(WITH_TESTS)
    target_link_libraries(gcm3d_unit_tests gcm)
    target_link_libraries(gcm3d_unit_tests ${GTEST_LIBRARIES})

    target_link_libraries(gcm3d_func_tests gcm)
    target_link_libraries(gcm3d_func_tests ${GTEST_LIBRARIES})
endif()

install(
    TARGETS gcm gcm3d
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)

install(
    FILES tools/pv_render.py DESTINATION bin RENAME gcm3d_pv_render.py
)
