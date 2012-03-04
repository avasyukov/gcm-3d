cmake_minimum_required (VERSION 2.8)

set (PROJECT gcm3d)
project (${PROJECT})

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/CMakeModules)

if (CMAKE_COMPILER_IS_GNUCXX)
	message (STATUS "Adding -Wno-deprecated to supress warnings from VTK.")
	add_definitions (-Wno-deprecated)
else ()
	message (WARNING "Compiler untested!")
endif ()

add_executable (${PROJECT} main.cpp)
set_target_properties(${PROJECT} PROPERTIES COMPILE_FLAGS -O3)

add_executable (${PROJECT}-profile EXCLUDE_FROM_ALL main.cpp)
set_target_properties(${PROJECT}-profile PROPERTIES COMPILE_FLAGS -pg)

find_package (VTK REQUIRED)
if (VTK_FOUND)
	message(STATUS "VTK found")
	include_directories(${VTK_INCLUDE_DIRS})
	set(LINK_LIBRARIES vtkCommon vtkVolumeRendering vtkFiltering vtkIO)
endif (VTK_FOUND)

find_package (GSL REQUIRED)
if (GSL_FOUND)
	message(STATUS "GSL found")
	include_directories(${GSL_INCLUDE_DIRS})
	set(LINK_LIBRARIES ${LINK_LIBRARIES} ${GSL_LIBRARIES})
endif (GSL_FOUND)

find_package (TinyXML REQUIRED)
if(TINYXML_FOUND)
	message(STATUS "TinyXML found")
	include_directories(${TINYXML_INCLUDE_DIR})
	set(LINK_LIBRARIES ${LINK_LIBRARIES} ${TINYXML_LIBRARIES})
endif(TINYXML_FOUND)

set(LINK_LIBRARIES ${LINK_LIBRARIES} m)

target_link_libraries (${PROJECT} ${LINK_LIBRARIES})
target_link_libraries (${PROJECT}-profile ${LINK_LIBRARIES})
