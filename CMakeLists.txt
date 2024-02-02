##
#  CMakeLists.txt
#
#  Copyright (C) 2011-2021  SDML (www.srcML.org)
#
#  This file is part of the srcDiff translator.
#
#  The srcDiff translator is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  The srcDiff translator is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with the srcDiff translator; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

cmake_minimum_required(VERSION 3.14)
project(srcDiff)

if(NOT WIN32)
set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()

enable_testing()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)
include(srcdiff)


if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

    find_package(LibXml2 REQUIRED)
    find_package(Iconv REQUIRED)
    include_directories(${LIBXML2_INCLUDE_DIR})
    
    find_library(LIBSRCML_LIBRARY NAMES libsrcml.so)
    find_package(Boost COMPONENTS program_options filesystem system regex date_time REQUIRED)

else()

    # TODO: replace hardcoded dep path with vcpkg
    set(WINDOWS_DEP_PATH ${PROJECT_SOURCE_DIR}/src/windows/deps)
    
    include_directories(${WINDOWS_DEP_PATH}/include)
    link_directories(${WINDOWS_DEP_PATH}/$(ConfigurationName)/lib)
    set(LIBXML2_LIBRARIES libxml2.lib libiconv.lib)
    
    set(Boost_NO_SYSTEM_PATHS ON)
    set(Boost_LIBRARY_DIR_RELEASE ${WINDOWS_DEP_PATH}/release/lib)
    set(Boost_LIBRARY_DIR_DEBUG ${WINDOWS_DEP_PATH}/debug/lib)
    set(BOOST_INCLUDE_DIR ${WINDOWS_DEP_PATH}/include)
    set(BOOST_INCLUDEDIR ${WINDOWS_DEP_PATH}/include)
    set(Boost_INCLUDE_DIRS ${WINDOWS_DEP_PATH}/include)

    find_library(LIBSRCML_LIBRARY NAMES libsrcml PATHS "C:/Program Files/srcML/lib")
    include_directories("C:/Program Files/srcML/include")
    message(STATUS ${LIBSRCML_LIBRARY})


endif()

set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# setup project includes
include_directories(src/shortest_edit_script
					src/translator
					src/model
					src/client
					src/srcml
					src/windows
					src/project
					src/view
					src/summary
					
					src/srcSAX/src/srcsax
					src/srcSAX/src/cpp
					src/srcSAX/src/windows)

# include sub directories
add_subdirectory(src)

if(BUILD_TRANSLATOR_TESTS OR BUILD_UNIT_TESTS)
	add_subdirectory(test)
endif()
