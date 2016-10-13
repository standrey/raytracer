cmake_minimum_required (VERSION 3.5)
project (raytracer)
# The version number.
set(raytracer_VERSION_MAJOR 1)
set(raytracer_VERSION_MINOR 0)
set(CMAKE_BUILD_TYPE Debug)


#INCLUDE(FindPkgConfig)
#PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)



# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "include/config.h.in"
  "include/config.h"
  )
 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h


file(GLOB SOURCES "src/*.cpp")
 
# add the executable
add_executable(raytracer ${SOURCES})

find_package(OpenMP REQUIRED)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})

#find_package(SDL2IMAGE REQUIRED)
#if (NOT SDL2IMAGE_FOUND)
#    message(ERROR, "SDL2_image not found")
#endif()

include_directories( "include" )
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS})
include_directories( ${PROJECT_SOURCE_DIR}/../glm )
target_link_libraries( raytracer ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})