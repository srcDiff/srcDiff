##
#  config.cmake
#
#  Copyright (C) 2011-2011 SDML (www.sdml.info)
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
option(BUILD_UNIT_TESTS "Build unit tests for srcDiff" OFF)

option(SVN_ENABLED "Build in svn source input support" OFF)

if(SVN_ENABLED)

find_package(LibApr REQUIRED)
find_package(LibSvn REQUIRED)

add_definitions(-DSVN)

endif()

set(CMAKE_CXX_FLAGS "-std=c++11")

# find needed libraries
find_library(LIBSRCML_LIBRARY NAMES libsrcml.dll libsrcml.a PATHS /usr/local/lib)
set(LIBSRCML_INCLUDE_DIR /usr/local/include)

set(Boost_NO_BOOST_CMAKE ON)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS program_options filesystem system thread regex date_time REQUIRED)

find_package(LibXml2 REQUIRED)

find_library(ANTLR_LIBRARY NAMES libantlr-pic.a libantlr.a libantlr2-0.dll antlr.lib PATHS /usr/lib /usr/local/lib)

# Set libsrcdiff libraries
set(LIBSRCDIFF_LIBRARIES ${LIBSRCML_LIBRARY} ${Boost_LIBRARIES} ${LIBXML2_LIBRARIES} ${ANTLR_LIBRARY} pthread ${LIBAPR_LIBRARIES} ${LIBSVN_LIBRARIES} CACHE INTERNAL "libsrcdiff Link Libraries")

# Set libsrcdiff libraries
set(SRCDIFF_LIBRARIES crypto CACHE INTERNAL "srcdiff Link Libraries")

# Do not use rpath on OSX
set(CMAKE_MACOSX_RPATH OFF)

if(NOT WIN32 AND NOT APPLE)
list(APPEND SRCDIFF_LIBRARIES rt)
endif()

# include needed includes
include_directories(${LIBSRCML_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR} ${LIBAPR_INCLUDE_DIR} ${LIBSVN_INCLUDE_DIR})

