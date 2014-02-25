###
#    CMakeLists.txt
#
#
#    Optional cmake file for srcML.
#    The main way to build srcML is with normal Makefile

cmake_minimum_required(VERSION 2.8)
project(srcDiff)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)
include(config)

# setup project includes
include_directories(src/shortest_edit_script src/translator src/windows src/oldclient src/srcml	src/visualization)

#set_source_files_properties(shortest_edit_script.c PROPERTIES LANGUAGE CXX)

# get version and revision
add_definitions(-DVERSION=\"1\")
add_definitions(-DREVISION=\"1\")

# include sub directories
subdirs(src)
