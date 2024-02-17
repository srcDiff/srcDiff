##
#  config.cmake
#
#  Copyright (C) 2011-2021 SDML (www.srcML.org)
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
#
#  Build configuration file

# build options
option(BUILD_TRANSLATOR_TESTS "Build files for testing srcDiff translator" ON)

include_directories(${LIBSRCML_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR}  ${LIBGIT2_INCLUDE_DIR})
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (WIN32)
    add_compile_definitions(WIN32)
endif()

if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${GCC_WARNINGS} -fpermissive")
    set(CMAKE_C_FLAGS "${GCC_WARNINGS}")
    if (WIN32)
        message(STATUS "Building with Windows but not with MSVC")
        # pretend we're using msvc anyway for now:
        add_compile_definitions(_MSC_BUILD)
    endif()
else()
    set(CMAKE_CXX_FLAGS "/permissive- /EHsc -wd4068")
endif()
