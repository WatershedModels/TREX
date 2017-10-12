# CMake build for Windows, Linux, and macOS
# by Liang-Jun Zhu
# zlj@lreis.ac.cn
cmake_minimum_required(VERSION 3.0)
## TREX version
set(TREX_VER_MAJOR 2011)
set(TREX_VER_MINOR 0210)
## Project name and language
set(prjname TREX${TREX_VER_MAJOR}rev${TREX_VER_MINOR})
project(${prjname} C)
################# Install directory ##################
IF (INSTALL_PREFIX)
    SET(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX})
    SET(INSTALL_DIR ${INSTALL_PREFIX})
ELSE ()
    SET(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})
    SET(INSTALL_DIR ${CMAKE_SOURCE_DIR}/bin)
ENDIF ()
################# OS platform   ##################
if (WIN32)
    add_definitions(-Dwindows)
    if (MSVC)
        add_definitions(-DMSVC)
    endif ()
elseif (APPLE)
    # -- Determine the version of OSX
    # -- 8 and less are OSX 10.0 - 10.4
    # -- 9 is 10.5 (LEOPARD)
    EXEC_PROGRAM(uname ARGS -v OUTPUT_VARIABLE DARWIN_VERSION)
    STRING(REGEX MATCH "[0-9]+" DARWIN_VERSION ${DARWIN_VERSION})
    MESSAGE(STATUS "DARWIN_VERSION=${DARWIN_VERSION}")
    if (DARWIN_VERSION GREATER 8)
        add_definitions(-Dmacos)
    else ()
        add_definitions(-Dmacosold)
    endif ()
else ()
    add_definitions(-Dlinux)
endif ()
# Add support for OpenMP (if supported)
find_package(OpenMP)
IF (OPENMP_FOUND)
    add_definitions(-DSUPPORT_OMP)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
ENDIF ()
# Set compile flags

add_subdirectory(trex_src/src)
