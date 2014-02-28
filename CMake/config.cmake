##
#  config.cmake
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
#
#  Build configuration file


find_package(LibXml2 REQUIRED)
find_library(LIBANTLR_LIB NAMES libantlr-pic.a libantlr.a libantlr2-0.dll PATHS /usr/lib /usr/local/lib ../dlls)
find_library(LIBSRCML_LIB NAMES libsrcml.dll libsrcml.dylib libsrcml.so PATHS /usr/local/lib)
set(LIBSRCML_INCLUDE_DIR /usr/local/include)

include_directories(${LIBXML2_INCLUDE_DIR} ${LIBSRCML_INCLUDE_DIR})

set_property(GLOBAL PROPERTY LIBXML2_LIBS ${LIBXML2_LIBRARIES})
set_property(GLOBAL PROPERTY LIBSRCML_LIB ${LIBSRCML_LIB})
set_property(GLOBAL PROPERTY LIBANTLR_LIB ${LIBANTLR_LIB})

# define needed programs
find_program(ANTLR NAMES antlr runantlr cantlr antlr2 PATHS /usr/bin /opt/local/bin /usr/local/bin)
set_property(GLOBAL PROPERTY ANTLR ${ANTLR})


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin )
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../bin )
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../bin )
