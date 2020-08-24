##
#  CMakeLists.txt
#
#  Copyright (C) 2011-2011  SDML (www.sdml.info)
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

cmake_minimum_required(VERSION 2.8)
project(srcDiff)

enable_testing()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)
include(srcdiff)

# setup project includes
include_directories(src/shortest_edit_script src/translator src/windows src/client src/srcml
					src/view src/summary src/project
					src/srcSAX/src/srcsax src/srcSAX/src/cpp src/srcSAX/src/windows)

# include sub directories
add_subdirectory(src)

if(BUILD_TRANSLATOR_TESTS)
    	include_directories(test/unit)
	add_subdirectory(test)
    
endif()

