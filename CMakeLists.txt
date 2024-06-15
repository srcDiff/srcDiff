## SPDX-License-Identifier: GPL-3.0-only
#	 CMakeLists.txt
#
#  @copyright Copyright (C) 2011-2024 SDML (www.srcDiff.org)
#
#  This file is part of the srcDiff Infrastructure.
#

cmake_minimum_required(VERSION 3.28)

project(srcDiff)

if(CMAKE_USE_VCPKG)
  message(STATUS "Building with vcpkg")
  if(DEFINED ENV{VCPKG_ROOT})
  	include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
  else()
	message(SEND_ERROR "Environment variable VCPKG_ROOT not defined!")
  endif()
endif()

# this eliminates a warning about ambiguous linker search paths on linux by
# explicitly deactivating some legacy CMake behavior
# https://cmake.org/cmake/help/latest/policy/CMP0003.html
if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if(NOT WIN32)
set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()

enable_testing()

option(BUILD_UNIT_TESTS "Build all unit tests" OFF)
option(BUILD_TRANSLATOR_TESTS "Build all transltor tests" ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 20)
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
