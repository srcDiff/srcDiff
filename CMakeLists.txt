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

cmake_minimum_required(VERSION 3.27)
project(srcDiff)

if(NOT WIN32)
set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()

enable_testing()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)

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

