cmake_minimum_required (VERSION 3.5)
project (raytracer)
# The version number.
set (raytracer_VERSION_MAJOR 1)
set (raytracer_VERSION_MINOR 0)
set (CMAKE_BUILD_TYPE Debug)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "include/config.h.in"
  "include/config.h"
  )
 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("include")

file(GLOB SOURCES "src/*.cpp")
 
# add the executable
add_executable(RayTracer ${SOURCES})

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
