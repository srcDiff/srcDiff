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

# find needed libraries
find_package(LibXml2 REQUIRED)
find_library(LIBSRCML_LIBRARY NAMES libsrcml.dll libsrcml.dylib libsrcml.so PATHS /usr/local/lib)
set(LIBSRCML_INCLUDE_DIR /usr/local/include)

# include needed includes
include_directories(${LIBXML2_INCLUDE_DIR} ${LIBSRCML_INCLUDE_DIR})

