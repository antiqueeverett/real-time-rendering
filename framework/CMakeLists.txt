############################################################
# CMake Build Script
############################################################

# Project settings
cmake_minimum_required(VERSION 2.8.7)
project(rtr)

set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE )

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinsizeRel."
        FORCE)
endif()

get_filename_component(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} ABSOLUTE)

message(STATUS "Build type:     ${CMAKE_BUILD_TYPE}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")

############################################################
# Directories

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
set(THIRD_PARTY_PROJECTS_DIR ${CMAKE_SOURCE_DIR}/third_party)
set(GLOBAL_EXT_DIR ${CMAKE_SOURCE_DIR}/externals)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(TEST_OUTPUT_PATH ${CMAKE_BINARY_DIR}/test)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(RTR_CONFIG_DIR ${CMAKE_BINARY_DIR})

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install" CACHE PATH "default install path" FORCE )
endif()


file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
file(MAKE_DIRECTORY ${TEST_OUTPUT_PATH})
file(MAKE_DIRECTORY ${LIBRARY_OUTPUT_PATH})

############################################################
if(MSVC)
    set(PROJECT_COMPILE_FLAGS "-D_VARIADIC_MAX=10 -D _SCL_SECURE=0 -D _USE_MATH_DEFINES=1 /MP -D NOMINMAX")
    set(PROJECT_LIBS "")
elseif(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(PROJECT_COMPILE_FLAGS "-std=c++1y -Wall -pthread -fPIC -fopenmp")
    set(PROJECT_LIBS "pthread")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PROJECT_COMPILE_FLAGS}")

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")

############################################################
# Externals
################################
SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${GLOBAL_EXT_DIR})

################################
# schism
################################
#include(find_schism)

#LINK_DIRECTORIES(${SCHISM_LIBRARY_DIRS})

################################
# Boost
################################
IF (MSVC)
  set(Boost_USE_MULTITHREADED ON)
  set(Boost_USE_STATIC_LIBS ON)
ENDIF ()

IF (MSVC)
  set(BOOST_INCLUDEDIR ${GLOBAL_EXT_DIR}/boost/include)
  set(BOOST_LIBRARYDIR ${GLOBAL_EXT_DIR}/boost/lib)
ELSEIF (UNIX)
  set(BOOST_INCLUDEDIR /opt/boost/latest/include)
  set(BOOST_LIBRARYDIR /opt/boost/latest/lib)
ENDIF ()

find_package( Boost 1.54.0 REQUIRED thread system program_options timer chrono filesystem date_time)
find_package( Threads )

ADD_DEFINITIONS("-DBOOST_ALL_NO_LIB")

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
LINK_DIRECTORIES(${Boost_LIBRARY_DIR})

MESSAGE( STATUS "BOOST INCLUDE DIR: " ${Boost_INCLUDE_DIR} )
MESSAGE( STATUS "BOOST LIBRARY DIR: " ${Boost_LIBRARY_DIR} )

################################
# GLUT
################################
include(FindGLUT)

################################
# GLFW
################################
#include(findGLFW3)


################################
# Awesomium
################################
#include(FindAwesomium)

################################
# OpenGL
################################
include(FindOpenGL)

################################
# OpenCV
################################
#include(FindOpenCV)

# Include directories to add to the user project:
# ======================================================

#SET(OpenCV_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/externals/opencv-3.1.0/install/include;${CMAKE_CURRENT_SOURCE_DIR}/externals/opencv-3.1.0/install/include")
#INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

# ======================================================
# Link directories to add to the user project:
# ======================================================

# Provide the libs directory anyway, it may be needed in some cases.

#SET(OpenCV_LIB_DIR "${OpenCV_INSTALL_PATH}/lib")

#SET(OpenCV_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/externals/opencv-3.1.0/install/lib")


#LINK_DIRECTORIES(${OpenCV_LIB_DIR})

#FILE(GLOB OpenCVLIBS ${CMAKE_CURRENT_SOURCE_DIR}/externals/opencv-3.1.0/install/lib/*.so)

###############################

###############################
# X11
###############################
include(find_x11)

###############################
# GLEW
###############################
include(FindGLEW)

################################ 
# FreeImage
################################
include(find_freeimage)

################################ 
# SDL2
################################
include(FindSDL2)


################################ 
# SOIL
################################
include(FindSOIL)

################################ 
# ASSIMP
################################
include(FindASSIMP)

################################ 
# DevIL
################################
include(FindDevIL)

#FIND_PACKAGE(CUDA REQUIRED)


############################################################
# Build 3rd-party
############################################################
# Targets

add_subdirectory(${CMAKE_SOURCE_DIR}/rendering)
add_subdirectory(${CMAKE_SOURCE_DIR}/apps)


# install shaders
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders DESTINATION resources
        FILES_MATCHING 
        PATTERN "*.vert"
        PATTERN "*.geom"
        PATTERN "*.frag"
        PATTERN "*.tess"
    	PATTERN "*.vs"
    	PATTERN "*.fs"
        PATTERN "*.glsl"
        )

# install textures
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures DESTINATION resources
        FILES_MATCHING 
        PATTERN "*.png"
        PATTERN "*.PNG"
        PATTERN "*.jpg"
        PATTERN "*.JPG"
        PATTERN "*.jpeg"
        PATTERN "*.JPEG"
        PATTERN "*.tiff"
        PATTERN "*.TIFF"
        PATTERN "*.bmp"
        PATTERN "*.BMP"
        )

# install objects
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources/objects DESTINATION resources
        FILES_MATCHING 
        PATTERN "*.obj"
        PATTERN "*.OBJ"
        PATTERN "*.dae"
        PATTERN "*.DAE"
        PATTERN "*.fbx"
        PATTERN "*.FBX"
        )

#install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/fonts/Ubuntu.ttf DESTINATION share/rtr/fonts)
